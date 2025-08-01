/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-present Facebook, Inc. (http://www.facebook.com)  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#include "hphp/compiler/package.h"

#include <exception>
#include <filesystem>
#include <memory>
#include <set>
#include <sys/stat.h>
#include <sys/types.h>
#include <utility>
#include <vector>

#include <folly/String.h>
#include <folly/portability/Dirent.h>
#include <folly/portability/Unistd.h>

#include "hphp/compiler/decl-provider.h"
#include "hphp/compiler/option.h"
#include "hphp/hack/src/hackc/ffi_bridge/compiler_ffi.rs.h"
#include "hphp/hhvm/process-init.h"
#include "hphp/runtime/base/file-util-defs.h"
#include "hphp/runtime/base/file-util.h"
#include "hphp/runtime/base/program-functions.h"
#include "hphp/runtime/base/static-string-table.h"
#include "hphp/runtime/vm/as.h"
#include "hphp/runtime/vm/func-emitter.h"
#include "hphp/runtime/vm/preclass-emitter.h"
#include "hphp/runtime/vm/type-alias-emitter.h"
#include "hphp/runtime/vm/unit-emitter.h"
#include "hphp/runtime/vm/unit-parser.h"
#include "hphp/util/configs/eval.h"
#include "hphp/util/exception.h"
#include "hphp/util/extern-worker.h"
#include "hphp/util/hash.h"
#include "hphp/util/logger.h"
#include "hphp/util/multiglob.h"
#include "hphp/util/timer.h"
#include "hphp/util/virtual-file-system.h"
#include "hphp/zend/zend-string.h"

using namespace HPHP;
using namespace extern_worker;

namespace coro = folly::coro;

///////////////////////////////////////////////////////////////////////////////

const StaticString s_EntryPoint("__EntryPoint");

///////////////////////////////////////////////////////////////////////////////

Package::Package(const std::string& root,
                 TicketExecutor& executor,
                 extern_worker::Client& client,
                 bool coredump)
  : m_root{root}
  , m_failed{false}
  , m_total{0}
  , m_executor{executor}
  , m_client{client}
  , m_config{
      [this, coredump] { return m_client.store(Config::make(coredump)); },
      m_executor.sticky()
    }
  , m_repoOptions{client}
{
}

void Package::addInputList(const std::string& listFileName) {
  assert(!listFileName.empty());
  auto const f = fopen(listFileName.c_str(), "r");
  if (f == nullptr) {
    throw Exception("Unable to open %s: %s", listFileName.c_str(),
                    folly::errnoStr(errno).c_str());
  }
  char fileName[PATH_MAX];
  while (fgets(fileName, sizeof(fileName), f)) {
    int len = strlen(fileName);
    if (fileName[len - 1] == '\n') fileName[len - 1] = '\0';
    len = strlen(fileName);
    if (len) {
      if (FileUtil::isDirSeparator(fileName[len - 1])) {
        addDirectory(fileName);
      } else {
        addSourceFile(fileName);
      }
    }
  }
  fclose(f);
}

void Package::addStaticFile(const std::string& fileName) {
  assert(!fileName.empty());
  m_extraStaticFiles.insert(fileName);
}

void Package::addStaticDirectory(const std::string& path) {
  m_staticDirectories.insert(path);
}

void Package::addStaticPattern(const std::string& pattern) {
  assert(!pattern.empty());
  m_staticPatterns.insert(pattern);
}

void Package::addDirectory(const std::string& path) {
  m_directories.emplace(path);
}

void Package::addSourceFile(const std::string& fileName) {
  if (fileName.empty()) return;
  auto canonFileName =
    FileUtil::canonicalize(String(fileName)).toCppString();
  m_filesToParse.emplace(std::move(canonFileName), true);
}

void Package::writeVirtualFileSystem(const std::string& path) {
  auto writer = VirtualFileSystemWriter(path);

  if (!m_staticPatterns.empty()) {
    auto root = std::filesystem::path(m_root);
    auto files = MultiGlob::matches(m_staticPatterns, root);
    for (auto& file : files) {
      auto const rpath = std::filesystem::relative(file, root);
      if (writer.addFile(rpath.c_str(), file.c_str())) {
        Logger::Verbose("saving %s", file.c_str());
      }
    }
  } else {
    for (auto const& dir : m_directories) {
      std::vector<std::string> files;
      FileUtil::find(files, m_root, dir, /* php */ false, /* failHard */ true,
                    &Option::PackageExcludeStaticDirs,
                    &Option::PackageExcludeStaticFiles);
      Option::FilterFiles(files, Option::PackageExcludeStaticPatterns);
      for (auto& file : files) {
        auto const rpath = file.substr(m_root.size());
        if (writer.addFile(rpath.c_str(), file.c_str())) {
          Logger::Verbose("saving %s", file.c_str());
        }
      }
    }

    for (auto const& dir : m_staticDirectories) {
      std::vector<std::string> files;
      FileUtil::find(files, m_root, dir, /* php */ false, /* failHard */ true);
      for (auto& file : files) {
        auto const rpath = file.substr(m_root.size());
        if (writer.addFile(rpath.c_str(), file.c_str())) {
          Logger::Verbose("saving %s", file.c_str());
        }
      }
    }

    for (auto const& file : m_extraStaticFiles) {
      auto const fullpath = m_root + file;
      if (writer.addFile(file.c_str(), fullpath.c_str())) {
        Logger::Verbose("saving %s", fullpath.c_str());
      }
    }
  }

  for (auto const& pair : m_discoveredStaticFiles) {
    auto const file = pair.first.c_str();
    const char *fullpath = pair.second.c_str();
    if (fullpath[0]) {
      if (writer.addFile(file, fullpath)) {
        Logger::Verbose("saving %s", fullpath);
      }
    } else {
      if (writer.addFileWithoutContent(file)) {
        Logger::Verbose("saving %s", file);
      }
    }
  }
  writer.finish();
}

///////////////////////////////////////////////////////////////////////////////

namespace {

///////////////////////////////////////////////////////////////////////////////

std::unique_ptr<UnitEmitter>
createSymlinkWrapper(const std::string& fileName,
                     const std::string& targetPath,
                     std::unique_ptr<UnitEmitter> origUE) {
  auto found = false;

  std::ostringstream ss;
  for (auto const& fe : origUE->fevec()) {
    auto const& attrs = fe->userAttributes;
    if (attrs.find(s_EntryPoint.get()) != attrs.end()) {
      found = true;
      std::string escapedName;
      folly::cEscape(fe->name->toCppString(), escapedName);
      ss << ".function [persistent "
        "\"__EntryPoint\"(\"\"\"y:0:{}\"\"\")] (4,7) \"\" <N> "
        "entrypoint$symlink$" << string_sha1(fileName) << "() {\n"
         << "  String \"" << targetPath << "\"\n"
         << "  ReqOnce\n"
         << "  PopC\n"
         << "  NullUninit\n"
         << "  NullUninit\n"
         << "  FCallFuncD <> 0 1 \"\" \"\" - \"\" \"" << escapedName << "\"\n"
         << "  PopC\n"
         << "  Null\n"
         << "  RetC\n"
         << "}\n\n";
      break;
    }
  }
  if (!found) return nullptr;

  auto const content = ss.str();
  return assemble_string(
    content.data(),
    content.size(),
    fileName.c_str(),
    SHA1{string_sha1(content)},
    nullptr,
    origUE->m_packageInfo,
    false
  );
}

///////////////////////////////////////////////////////////////////////////////

const StaticString
  s_PackageOverride("__PackageOverride");

Package::FileMetaVec s_fileMetas;
Package::ParseMetaVec s_parseMetas;
Package::IndexMetaVec s_indexMetas;

size_t s_fileMetasIdx{0};

// Construct parse metadata for the given unit-emitter
UnitEmitterSerdeWrapper output(
  std::unique_ptr<UnitEmitter> ue,
  std::unique_ptr<Package::DeclNames> missing,
  const StringData* filepath
) {
  Package::ParseMeta meta;
  if (missing) meta.m_missing = std::move(*missing);
  if (!ue) {
    meta.m_filepath = filepath;
    s_parseMetas.emplace_back(std::move(meta));
    return UnitEmitterSerdeWrapper{};
  }

  meta.m_symbol_refs = std::move(ue->m_symbol_refs);
  meta.m_filepath = ue->m_filepath;
  auto const hasPackageOverride =
    ue->m_fileAttributes.find(s_PackageOverride.get());
  if (hasPackageOverride != ue->m_fileAttributes.end()) {
    assertx(tvIsVec(hasPackageOverride->second)
            && val(hasPackageOverride->second).parr->size() == 1);
    meta.m_packageOverride =
      tvAssertStringLike(val(hasPackageOverride->second).parr->at(int64_t(0)));
  }
  meta.m_module_use = ue->m_moduleName;

  for (auto const pce : ue->preclasses()) {
    if (pce->attrs() & AttrEnum) {
      meta.m_definitions.m_enums.emplace_back(pce->name());
    } else {
      meta.m_definitions.m_classes.emplace_back(pce->name());
    }
  }
  for (auto const& fe : ue->fevec()) {
    if (fe->attrs & AttrIsMethCaller) {
      meta.m_definitions.m_methCallers.emplace_back(fe->name);
    } else {
      meta.m_definitions.m_funcs.emplace_back(fe->name);
    }
  }
  for (auto const& t : ue->typeAliases()) {
    meta.m_definitions.m_typeAliases.emplace_back(t->name());
  }
  for (auto const& c : ue->constants()) {
    meta.m_definitions.m_constants.emplace_back(c.name);
  }
  for (auto const& m : ue->modules()) {
    meta.m_definitions.m_modules.emplace_back(m.name);
  }

  s_parseMetas.emplace_back(std::move(meta));
  return std::move(ue);
}

// HHVM shutdown code shared by different Job types.
void finishJob() {
  hphp_process_exit();
  rds::local::fini();
}

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////

void Package::parseInit(const Config& config, FileMetaVec meta) {
  if (!config.CoreDump) {
    struct rlimit rl{};
    rl.rlim_cur = 0;
    rl.rlim_max = 0;
    setrlimit(RLIMIT_CORE, &rl);
  }

  rds::local::init();

  Hdf hdf;
  IniSetting::Map ini = IniSetting::Map::object;
  RO::Load(ini, hdf);

  config.apply();
  Logger::LogLevel = Logger::LogError;

  // Inhibit extensions and systemlib from being initialized. It
  // takes a while and we don't need it.
  register_process_init();
  hphp_process_init(true);

  // Don't use unit emitter's caching here, we're relying on
  // extern-worker to do that for us.
  g_unit_emitter_cache_hook = nullptr;

  s_fileMetas = std::move(meta);
  s_fileMetasIdx = 0;
}

Package::ParseMetaVec Package::parseFini() {
  assertx(s_fileMetasIdx == s_fileMetas.size());
  assertx(s_parseMetas.size() == s_fileMetas.size());
  finishJob();
  return std::move(s_parseMetas);
}

UnitEmitterSerdeWrapper
Package::parseRun(const std::string& content,
                  const RepoOptionsFlags& repoOptions,
                  const std::vector<UnitDecls>& decls) {
  if (s_fileMetasIdx >= s_fileMetas.size()) {
    throw Error{
      folly::sformat("Encountered {} inputs, but only {} file metas",
                     s_fileMetasIdx+1, s_fileMetas.size())
    };
  }
  auto const& meta = s_fileMetas[s_fileMetasIdx++];
  auto const& fileName = meta.m_filename;

  try {
    if (Cfg::Eval::AllowHhas && folly::StringPiece(fileName).endsWith(".hhas")) {
      auto ue = assemble_string(
        content.data(),
        content.size(),
        fileName.c_str(),
        SHA1{string_sha1(content)},
        nullptr,
        repoOptions.packageInfo()
      );
      if (meta.m_targetPath) {
        ue = createSymlinkWrapper(
          fileName, *meta.m_targetPath, std::move(ue)
        );
        if (!ue) {
          // If the symlink contains no EntryPoint we don't do
          // anything but it is still success
          return output(nullptr, nullptr, makeStaticString(fileName));
        }
      }
      // Assembling hhas never emits DeclNames.
      return output(std::move(ue), nullptr, nullptr);
    }

    SHA1 mangled_sha1{
      mangleUnitSha1(string_sha1(content), fileName, repoOptions)
    };
    auto const mode =
      Cfg::Eval::AbortBuildOnCompilerError ? CompileAbortMode::AllErrors :
      Cfg::Eval::AbortBuildOnVerifyError   ? CompileAbortMode::VerifyErrors :
      CompileAbortMode::OnlyICE;

    std::unique_ptr<UnitEmitter> ue;
    BatchDeclProvider provider(decls);
    try {
      ue = compile_unit(
        content,
        CodeSource::User,
        fileName.c_str(),
        mangled_sha1,
        nullptr,
        /* isSystemLib */ false,
        /* forDebuggerEval */ false,
        repoOptions,
        mode,
        Cfg::Eval::EnableDecl ? &provider : nullptr
      );
    } catch (const CompilerAbort& exn) {
      ParseMeta meta;
      meta.m_abort = exn.what();
      meta.m_filepath = makeStaticString(fileName);
      s_parseMetas.emplace_back(std::move(meta));
      return UnitEmitterSerdeWrapper{};
    }

    if (ue) {
      if (!ue->m_ICE && meta.m_targetPath) {
        ue =
          createSymlinkWrapper(fileName, *meta.m_targetPath, std::move(ue));
        if (!ue) {
          // If the symlink contains no EntryPoint we don't do anything but it
          // is still success
          return output(nullptr, nullptr, makeStaticString(fileName));
        }
      }
      provider.finish();
      auto missing = std::make_unique<DeclNames>(std::move(provider.m_missing));
      return output(std::move(ue), std::move(missing), nullptr);
    } else {
      throw Error{
        folly::sformat("Unable to compile: {}", fileName)
      };
    }
  } catch (const std::exception& exn) {
    throw Error{
      folly::sformat("While parsing `{}`: {}", fileName, exn.what())
    };
  }
}

///////////////////////////////////////////////////////////////////////////////

/*
 * File grouping:
 *
 * Since every invocation of an extern-worker worker has some fixed
 * overhead, we want to parse multiple files per invocation. We also
 * want to leverage any caching that extern-worker has for job
 * execution. Since we assume that source files will change over time,
 * we don't want to group too many files together (if one file
 * changes, we'll have to reparse all of them in that
 * group). Furthermore, to maximize cache effectiveness, we want to
 * group files together in a deterministic way. Finally, there may be
 * different "subsections" of the source tree, which are only parsed
 * depending on the input files configeration (for example, some
 * builds may discard test directories and some might not). Again, we
 * want to maximize caching across these different "flavor" of builds
 * and try to avoid grouping together files from these different
 * subsets.
 *
 * We utilize the following scheme to try to accomplish all
 * this. First we define a group size (Option::ParserGroupSize). This
 * is the amount of files (on average) we'll group together in one
 * job. Input files are discovered by walking directories
 * recursively. We proceed bottom up. For every directory, we first
 * process its sub-directories. Each sub-directory processed returns
 * the groups it has already created (each roughly containing
 * Option::ParserGroupSize) files, along with any "left over" files
 * which have not been grouped. These results are all aggregated
 * together, and any files in the current directory are added to the
 * ungrouped set. If the number of files in the ungrouped set exceeds
 * Option::ParserDirGroupSizeLimit, then we attempt to group all of
 * those files.
 *
 * Grouping is done by hashing the files' names, and then using
 * consistent_hash to assign them to one of N buckets (where N is the
 * number of files divided by Option::ParserGroupSize rounded up). The
 * consistent hashing ensures that the minimal amount of disruption
 * happens when we add/remove files. (Adding one file will change
 * exactly one bucket, etc).
 *
 * If we grouped the files, they are returned to the parent directory
 * as groups (along with any groups from sub-directories). Otherwise
 * the files are returned as ungrouped and the process repeats in the
 * parent.
 *
 * The idea behind Option::ParserDirGroupSizeLimit is to try to
 * partition the source tree into distinct chunks and only grouping
 * files within those chunks. So, if you don't compile one of those
 * chunks (say because you're not compiling tests, for example), it
 * won't affect the files in other chunks. Otherwise if that test code
 * was mixed in with the rest of the groups, they'd all miss the cache
 * and have to be rerun. This is a heuristic, but in practice it seems
 * to work well.
 *
 * Once you reach the top level, any remaining ungrouped files (along
 * with any top level files added in by config) are grouped together.
 *
 * Before parsing, we sort all of the groups by their summed file
 * size. We want to start parsing larger groups first because they'll
 * probably take the longest.
 */

// Given the path of a directory, find all (relevant) files in that
// directory (and sub-directories), and attempt to group them.
coro::Task<Package::GroupResult> Package::groupDirectories(
  std::string path, bool filterFiles, bool filterDirs, bool failHard
) {
  // We're not going to be blocking on I/O here, so make sure we're
  // running on the thread pool.
  co_await coro::co_reschedule_on_current_executor;

  GroupResult result;
  std::vector<coro::Task<GroupResult>> dirs;

  FileUtil::find(
    m_root, path, /* php */ true, failHard,
    [&] (const std::string& name, bool dir, size_t size) {
      if (!dir) {
        if (filterFiles) {
          if (Option::PackageExcludeFiles.count(name) ||
              Option::IsFileExcluded(name, Option::PackageExcludePatterns)) {
            // files excluded with --exclude-file/--exclude-pattern are never
            // included in the build, so they are not tracked in the ignored set
            return false;
          }
        }

        if (!name.empty()) {
          auto canonFileName =
            FileUtil::canonicalize(String(name)).toCppString();
          if (m_seenFiles.emplace(std::move(canonFileName), true).second) {
            result.m_ungrouped.emplace_back(FileAndSize{name, size});
          }
        }
        return true;
      }
      if (filterDirs && Option::PackageExcludeDirs.count(name)) {
        // Only skip excluded dirs when requested.
        return false;
      }
      if (path == name ||
          (name.size() == path.size() + 1 &&
           name.back() == FileUtil::getDirSeparator() &&
           name.compare(0, path.size(), path) == 0)) {
        // find immediately calls us back with a canonicalized version
        // of path; we want to ignore that, and let it proceed to
        // iterate the directory.
        return true;
      }

      // Process the directory as a new job
      dirs.emplace_back(groupDirectories(name, filterFiles, filterDirs));

      // Don't iterate the directory in this job.
      return false;
    }
  );

  // Coalesce the sub results
  for (auto& sub : co_await coro::collectAllRange(std::move(dirs))) {
    result.m_grouped.insert(
      result.m_grouped.end(),
      std::make_move_iterator(sub.m_grouped.begin()),
      std::make_move_iterator(sub.m_grouped.end())
    );
    result.m_ungrouped.insert(
      result.m_ungrouped.end(),
      std::make_move_iterator(sub.m_ungrouped.begin()),
      std::make_move_iterator(sub.m_ungrouped.end())
    );
  }

  // Have we gathered enough files to assign them to groups?
  if (result.m_ungrouped.size() >= Option::ParserDirGroupSizeLimit) {
    groupFiles(result.m_grouped, std::move(result.m_ungrouped));
    assertx(result.m_ungrouped.empty());
  }
  co_return result;
}

// Group sets of files together using consistent hashing
void Package::groupFiles(Groups& groups, FileAndSizeVec files) {
  if (files.empty()) return;

  assertx(Option::ParserGroupSize > 0);
  // Number of buckets
  auto const numNew =
    (files.size() + (Option::ParserGroupSize - 1)) / Option::ParserGroupSize;

  auto const origSize = groups.size();
  groups.resize(origSize + numNew);

  // Assign to buckets by hash(filename)
  for (auto& [file, size] : files) {
    auto const idx = consistent_hash(
      hash_string_cs(file.c_str(), file.native().size()),
      numNew
    );
    assertx(idx < numNew);
    groups[origSize + idx].m_files.emplace_back(std::move(file));
    groups[origSize + idx].m_size += size;
  }

  // We could (though unlikely) have empty buckets. Remove them so we
  // don't have to deal with this when parsing.
  groups.erase(
    std::remove_if(
      groups.begin() + origSize,
      groups.end(),
      [] (const Group& g) { return g.m_files.empty(); }
    ),
    groups.end()
  );

  // Keep the order of the files within the bucket deterministic
  for (size_t i = origSize, n = groups.size(); i < n; ++i) {
    std::sort(groups[i].m_files.begin(), groups[i].m_files.end());
  }
}

// Parse all of the files in the given group
coro::Task<void> Package::parseGroups(
  Groups groups,
  const ParseCallback& callback,
  const UnitIndex& index
) {
  if (groups.empty()) co_return;

  // Kick off the parsing. Each group gets its own sticky ticket (so
  // earlier groups will get scheduling priority over later ones).
  std::vector<coro::TaskWithExecutor<void>> tasks;
  for (auto& group : groups) {
    tasks.emplace_back(
      co_withExecutor(m_executor.sticky(), parseGroup(std::move(group), callback, index))
    );
  }

  co_await coro::collectAllRange(std::move(tasks));
  co_return;
}

coro::Task<Package::Groups>
Package::groupAll(std::set<std::string>& directories,
    bool filterFiles, bool filterDirs, bool failHard) {
  Timer timer{Timer::WallTime, "finding inputs"};
  std::vector<coro::Task<GroupResult>> tasks;
  for (auto& dir : directories) {
    tasks.emplace_back(
      groupDirectories(std::move(dir), filterFiles, filterDirs, failHard)
    );
  }

  // Gather together all top level files
  GroupResult top;
  for (auto& result : co_await coro::collectAllRange(std::move(tasks))) {
    top.m_grouped.insert(
      top.m_grouped.end(),
      std::make_move_iterator(result.m_grouped.begin()),
      std::make_move_iterator(result.m_grouped.end())
    );
    top.m_ungrouped.insert(
      top.m_ungrouped.end(),
      std::make_move_iterator(result.m_ungrouped.begin()),
      std::make_move_iterator(result.m_ungrouped.end())
    );
  }

  // If there's any ungrouped files left over, group those now
  groupFiles(top.m_grouped, std::move(top.m_ungrouped));
  assertx(top.m_ungrouped.empty());

  // Finally add in any files explicitly added via configuration
  // and group them.
  FileAndSizeVec extraFiles;
  for (auto& file : m_filesToParse) {
    if (!m_seenFiles.insert(file).second) continue;
    extraFiles.emplace_back(FileAndSize{std::move(file.first), 0});
  }
  groupFiles(top.m_grouped, std::move(extraFiles));

  // Sort the groups from highest combined file size to lowest.
  // Larger groups will probably take longer to process, so we want to
  // start those earliest.
  std::sort(
    top.m_grouped.begin(),
    top.m_grouped.end(),
    [] (const Group& a, const Group& b) {
      if (a.m_size != b.m_size) return b.m_size < a.m_size;
      if (a.m_files.size() != b.m_files.size()) {
        return b.m_files.size() < a.m_files.size();
      }
      return a.m_files < b.m_files;
    }
  );

  co_return std::move(top.m_grouped);
}

coro::Task<void>
Package::parseAll(const ParseCallback& callback, const UnitIndex& index) {
  // Find the initial set of groups.
  auto groups = co_await groupAll(m_directories, true, false);

  // Parse all input files and autoload-eligible files
  Timer timer{Timer::WallTime, "parsing files"};
  co_await parseGroups(std::move(groups), callback, index);
  m_inputMicros = std::chrono::microseconds{timer.getMicroSeconds()};
  co_return;
}

coro::Task<bool> Package::parse(const UnitIndex& index,
                                const ParseCallback& callback) {
  assertx(callback);

  Logger::FInfo(
    "parsing using {} threads using {}",
    m_executor.numThreads(),
    m_client.implName()
  );

  co_await co_withExecutor(m_executor.sticky(), parseAll(callback, index));
  co_return !m_failed.load();
}

namespace {

std::string createFullPath(const std::filesystem::path& fileName,
    const std::string& root) {
  if (FileUtil::isDirSeparator(fileName.native().front())) {
    return fileName.native();
  }
  return root + fileName.native();
}

bool statFile(const std::filesystem::path& fileName, const std::string& root,
               std::string& fullPath, Optional<std::string>& targetPath) {
  fullPath = createFullPath(fileName, root);

  struct stat sb;
  if (lstat(fullPath.c_str(), &sb)) {
    if (fullPath.find(' ') == std::string::npos) {
      Logger::Error("Unable to stat file %s", fullPath.c_str());
    }
    return false;
  }
  if ((sb.st_mode & S_IFMT) == S_IFDIR) {
    Logger::Error("Unable to parse directory: %s", fullPath.c_str());
    return false;
  }
  if (S_ISLNK(sb.st_mode)) {
    auto const target = std::filesystem::canonical(fullPath);
    targetPath.emplace(std::filesystem::relative(target, root).native());
  }
  return true;
}

coro::Task<std::tuple<
  const Ref<Package::Config>*,
  Ref<std::vector<Package::FileMeta>>,
  std::vector<Ref<std::string>>,
  std::vector<Ref<RepoOptionsFlags>>
>>
storeInputs(
  bool optimistic,
  std::vector<std::filesystem::path>& paths,
  std::vector<Package::FileMeta>& metas,
  std::vector<coro::Task<Ref<RepoOptionsFlags>>>& options,
  Optional<std::vector<Ref<RepoOptionsFlags>>>& storedOptions,
  extern_worker::Client& client,
  const CoroAsyncValue<extern_worker::Ref<Package::Config>>& config
) {
  // Store the inputs and get their refs
  auto [fileRefs, metasRef, optionRefs, configRef] =
    co_await coro::collectAll(
      client.storeFile(paths, optimistic),
      optimistic
        ? client.storeOptimistically(metas)
        : client.store(metas),
      // If we already called storeInputs, then options will be
      // empty here (but storedOptions will be set).
      coro::collectAllRange(std::move(options)),
      *config
    );

  assertx(fileRefs.size() == paths.size());

  // Options are never stored optimistically. The first time we call
  // storeInputs, we'll store them above and store the results in
  // storedOptions. If we call this again, the above store for options
  // will do nothing, and we'll just reload the storedOptions.
  if (storedOptions) {
    assertx(!optimistic);
    assertx(optionRefs.empty());
    assertx(storedOptions->size() == paths.size());
    optionRefs = *std::move(storedOptions);
  } else {
    assertx(optionRefs.size() == paths.size());
    if (optimistic) storedOptions = optionRefs;
  }

  co_return std::make_tuple(
    configRef, metasRef, fileRefs, optionRefs
  );
}

}

coro::Task<void> Package::prepareInputs(
  Group group,
  std::vector<std::filesystem::path>& paths,
  std::vector<Package::FileMeta>& metas,
  std::vector<coro::Task<Ref<RepoOptionsFlags>>>& options
) {
  paths.reserve(group.m_files.size());
  metas.reserve(group.m_files.size());
  options.reserve(group.m_files.size());
  for (auto& fileName : group.m_files) {
    assertx(!fileName.empty());

    std::string fullPath;
    Optional<std::string> targetPath;
    if (!statFile(fileName, m_root, fullPath, targetPath)) {
      Logger::FError("Fatal: Unable to stat/parse {}", fileName.native());
      m_failed.store(true);
      continue;
    }

    // Most files will have the same RepoOptions, so we cache them
    auto const& repoOptions = RepoOptions::forFile(fullPath.data()).flags();
    options.emplace_back(
      m_repoOptions.get(
        repoOptions.cacheKeySha1(),
        repoOptions,
        co_await coro::co_current_executor
      )
    );
    paths.emplace_back(std::move(fullPath));
    metas.emplace_back(std::move(fileName), std::move(targetPath));
  }
  co_return;
}

// Parse a group using extern-worker and hand off the UnitEmitter or
// WPI::Key/Values obtained.
coro::Task<void> Package::parseGroup(
    Group group,
    const ParseCallback& callback,
    const UnitIndex& index
) {
  using namespace folly::gen;

  // Make sure we're running on the thread we should be
  co_await coro::co_reschedule_on_current_executor;

  try {
    // First build the inputs for the job
    std::vector<std::filesystem::path> paths;
    std::vector<FileMeta> metas;
    std::vector<coro::Task<Ref<RepoOptionsFlags>>> options;
    co_await prepareInputs(std::move(group), paths, metas, options);

    if (paths.empty()) {
      assertx(metas.empty());
      assertx(options.empty());
      co_return;
    }

    auto const workItems = paths.size();
    for (size_t i = 0; i < workItems; i++) {
      auto const& fileName = metas[i].m_filename;
      if (!m_extraStaticFiles.contains(fileName)) {
        m_discoveredStaticFiles.emplace(
          fileName,
          Option::CachePHPFile ? paths[i] : ""
        );
      }
    }

    Optional<std::vector<Ref<RepoOptionsFlags>>> storedOptions;
    // Try optimistic mode first. We won't actually store
    // anything, just generate the Refs. If something isn't
    // actually present in the workers, the execution will throw
    // an exception. If everything is present, we've skipped a
    // lot of work.
    bool optimistic = Option::ParserOptimisticStore &&
                      m_client.supportsOptimistic();
    for (;;) {
      auto [configRef, metasRef, fileRefs, optionsRefs] = co_await
        storeInputs(optimistic, paths, metas, options, storedOptions,
                    m_client, m_config);
      std::vector<FileData> fds;
      fds.reserve(workItems);
      for (size_t i = 0; i < workItems; i++) {
        fds.emplace_back(FileData {
            fileRefs[i],
            optionsRefs[i],
            {} // Start with no decls
        });
      }
      try {
        // When Eval.EnableDecl==true, parse jobs may return a list of
        // missing decl symbols. If any exist in UnitIndex, callback()
        // will return a nonempty ParseMetaVec, expecting a retry with
        // additional decls available from other files.
        // Retry until we successfully parse bytecode without requesting
        // any new decls.
        for (size_t attempts = 1;; ++attempts) {
          assertx(!metas.empty());
          Client::ExecMetadata metadata{
            .optimistic = optimistic,
            .job_key = folly::sformat("parse-{}-{}", attempts,
                                      metas[0].m_filename)
          };
          auto parseMetas = co_await callback(
            *configRef, metasRef, fds, metadata
          );
          if (parseMetas.empty()) {
            m_total += workItems;
            co_return;
          }
          always_assert(parseMetas.size() == workItems);
          // At least one item in the group needed additional decls.
          // Update its FileData with additional Ref<UnitDecls>.
          resolveDecls(index, metas, parseMetas, fds, attempts);
        }
      } catch (const extern_worker::WorkerError&) {
        throw;
      } catch (const extern_worker::Error&) {
        if (!optimistic) throw;
        optimistic = false;
      }
    }
  } catch (const Exception& e) {
    Logger::FError(
      "Fatal: An unexpected exception was thrown while parsing: {}",
      e.getMessage()
    );
    m_failed.store(true);
  } catch (const extern_worker::Error& e) {
    Logger::FError("Extern worker error while parsing: {}", e.what());
    m_failed.store(true);
  } catch (const std::exception& e) {
    Logger::FError(
      "Fatal: An unexpected exception was thrown while parsing: {}",
      e.what()
    );
    m_failed.store(true);
  } catch (...) {
    Logger::Error("Fatal: An unexpected exception was thrown while parsing");
    m_failed.store(true);
  }

  co_return;
}

///////////////////////////////////////////////////////////////////////////////

void Package::resolveDecls(
  const UnitIndex& index,
  const FileMetaVec& metas,
  const std::vector<ParseMeta>& parseMetas,
  std::vector<FileData>& fileDatas,
  size_t attempts
) {
  DEBUG_ONLY bool discovered = false;
  for (size_t i = 0, n = parseMetas.size(); i < n; i++) {
    auto& decls = std::get<2>(fileDatas[i]);
    auto origSize = decls.size();
    auto resolve = [&](auto const& names, auto const& table) {
      for (auto& name : names) {
        auto it = table.find(name);
        if (it != table.end()) {
          decls.emplace_back(it->second->declsRef);
        }
      }
    };
    auto const& missing = parseMetas[i].m_missing;
    resolve(missing.types, index.types);
    resolve(missing.funcs, index.funcs);
    resolve(missing.constants, index.constants);
    resolve(missing.modules, index.modules);

    // Remove dups
    std::sort(decls.begin(), decls.end());
    auto it = std::unique(decls.begin(), decls.end());
    decls.erase(it, decls.end());

    discovered |= decls.size() > origSize;
    if ((decls.size() > origSize && attempts > 1) || decls.size() > 100) {
      Logger::FVerbose("retry after attempts={} decls={} {}",
                    attempts, decls.size(), metas[i].m_filename);
    }
  }
  // If no new decls were discovered in any group item,
  // we would retry forever. Abort instead.
  assertx(discovered);
}

void Package::resolveOnDemand(OndemandInfo& out,
                              const StringData* fromFile,
                              const SymbolRefs& symbolRefs,
                              const UnitIndex& index,
                              bool report) {
  auto const& onPath = [&] (const std::string& rpath,
                            const StringData* sym) {
    if (rpath.empty()) return;
    if (Option::PackageExcludeFiles.contains(rpath) ||
        Option::IsFileExcluded(rpath, Option::PackageExcludePatterns)) {
      // Found symbol in UnitIndex, but the corresponding file was excluded.
      Logger::FVerbose("excluding ondemand file {}", rpath);
      return;
    }

    auto const toFile = makeStaticString(rpath);
    if (fromFile != toFile) {
      out.m_edges.emplace_back(SymbolRefEdge{sym, fromFile, toFile});
    }

    auto canon = FileUtil::canonicalize(String(rpath)).toCppString();
    assertx(!canon.empty());

    // Only emit a file once. This ensures we eventually run out
    // of things to emit.
    if (report || m_seenFiles.emplace(canon, true).second) {
      auto const absolute = [&] {
        if (FileUtil::isDirSeparator(canon.front())) {
          return canon;
        } else {
          return m_root + canon;
        }
      }();

      struct stat sb;
      if (stat(absolute.c_str(), &sb)) {
        Logger::FError("Unable to stat {}", absolute);
        m_failed.store(true);
        return;
      }
      out.m_files.emplace_back(FileAndSize{std::move(canon), (size_t)sb.st_size});
    }
  };

  auto const onMap = [&] (auto const& syms, auto const& sym_to_file) {
    for (auto const& sym : syms) {
      auto const s = makeStaticString(sym);
      auto const it = sym_to_file.find(s);
      if (it == sym_to_file.end()) continue;
      onPath(it->second->rpath, s);
    }
  };

  for (auto const& [kind, syms] : symbolRefs) {
    switch (kind) {
      case SymbolRef::Include:
        for (auto const& path : syms) {
          auto const rpath = path.compare(0, m_root.length(), m_root) == 0
            ? path.substr(m_root.length())
            : path;
          onPath(rpath, nullptr);
        }
        break;
      case SymbolRef::Class:
        onMap(syms, index.types);
        break;
      case SymbolRef::Function:
        onMap(syms, index.funcs);
        break;
      case SymbolRef::Constant:
        onMap(syms, index.constants);
        break;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

namespace {
// Extern-worker job for computing decls and autoload-index from source files.
struct IndexJob {
  static std::string name() { return "hphpc-index"; }

  static void init(const Package::Config& config,
                   Package::FileMetaVec meta) {
    Package::parseInit(config, std::move(meta));
  }

  static Package::IndexMetaVec fini() {
    return Package::indexFini();
  }

  static Package::UnitDecls run(
      const std::string& content,
      const RepoOptionsFlags& repoOptions
  );
};

Job<IndexJob> g_indexJob;
}

Package::IndexMetaVec Package::indexFini() {
  assertx(s_fileMetasIdx == s_fileMetas.size());
  assertx(s_indexMetas.size() == s_indexMetas.size());
  finishJob();
  return std::move(s_indexMetas);
}

// Index one source file:
// 1. compute decls
// 2. use facts from decls to compute IndexMeta (decl names in each file)
// 3. save serialized decls in UnitDecls as job output
Package::UnitDecls IndexJob::run(
    const std::string& content,
    const RepoOptionsFlags& repoOptions
) {
  if (s_fileMetasIdx >= s_fileMetas.size()) {
    throw Error{
      folly::sformat("Encountered {} inputs, but only {} file metas",
                     s_fileMetasIdx+1, s_fileMetas.size())
    };
  }
  auto const& meta = s_fileMetas[s_fileMetasIdx++];
  auto const& fileName = meta.m_filename;

  auto const bail = [&](const char* message) {
    Package::IndexMeta summary;
    summary.error = message;
    s_indexMetas.emplace_back(std::move(summary));
    return Package::UnitDecls{};
  };

  if (meta.m_targetPath) {
    // When/if this symlink is parsed, it's UnitEmitter will be a generated
    // stub function that calls the entrypoint of the target file (if any).
    // Don't index it since we don't expect any external references to the
    // generated stub entry point function.
    return bail("not indexing symlink");
  }

  if (Cfg::Eval::AllowHhas && folly::StringPiece(fileName).endsWith(".hhas")) {
    return bail("cannot index hhas");
  }

  hackc::DeclParserConfig decl_config;
  repoOptions.initDeclConfig(decl_config);
  auto decls = hackc::direct_decl_parse_and_serialize(
      decl_config,
      fileName,
      {(const uint8_t*)content.data(), content.size()}
  );
  if (decls.has_errors) {
    return bail("decl parser error");
  }

  // Get Facts from Decls, then populate IndexMeta.
  auto symbols = hackc::decls_to_symbols(*decls.decls);
  auto summary = summary_of_symbols(symbols);
  s_indexMetas.emplace_back(summary);
  if (!Cfg::Eval::EnableDecl) {
    // If decl-directed bytecode is disabled, parseRun() will not need
    // these decls, so don't bother storing them.
    return Package::UnitDecls{};
  }
  return Package::UnitDecls{
    std::move(summary),
    std::string{decls.serialized.begin(), decls.serialized.end()}
  };
}

coro::Task<bool> Package::index(const IndexCallback& callback) {
  Logger::FInfo(
    "indexing using {} threads using {}",
    m_executor.numThreads(),
    m_client.implName()
  );

  // TODO: index systemlib. But here is too late; they have already been
  // parsed into UEs at startup, and not yet claimed.
  co_await co_withExecutor(m_executor.sticky(), indexAll(callback));
  co_return !m_failed.load();
}

coro::Task<void> Package::indexAll(const IndexCallback& callback) {
  // If EnableDecl==true, all source files should be included in the
  // index, not just ondemand-eligible files.
  auto const filterFiles = !Cfg::Eval::EnableDecl;
  auto const filterDirs = false;

  // Compute the groups to index
  auto groups = co_await groupAll(m_directories, filterFiles, filterDirs);
  Logger::FInfo("indexing {:,} groups", groups.size());

  // Index all files
  Timer timer{Timer::WallTime, "indexing files"};
  co_await indexGroups(callback, std::move(groups));
  co_return;
}

// Index all of the files in the given groups
coro::Task<void> Package::indexGroups(const IndexCallback& callback,
                                      Groups groups) {
  if (groups.empty()) co_return;

  // Kick off indexing. Each group gets its own sticky ticket (so
  // earlier groups will get scheduling priority over later ones).
  std::vector<coro::TaskWithExecutor<void>> tasks;
  for (auto& group : groups) {
    tasks.emplace_back(
      co_withExecutor(m_executor.sticky(), indexGroup(callback, std::move(group)))
    );
  }
  co_await coro::collectAllRange(std::move(tasks));
  co_return;
}

// Index a group using extern-worker, invoke callback with each IndexMeta.
coro::Task<void> Package::indexGroup(const IndexCallback& callback,
                                     Group group) {
  using namespace folly::gen;

  // Make sure we're running on the thread we should be
  co_await coro::co_reschedule_on_current_executor;

  try {
    // First build the inputs for the job
    std::vector<std::filesystem::path> paths;
    std::vector<FileMeta> metas;
    std::vector<coro::Task<Ref<RepoOptionsFlags>>> options;
    co_await prepareInputs(std::move(group), paths, metas, options);

    if (paths.empty()) {
      assertx(metas.empty());
      assertx(options.empty());
      co_return;
    }

    auto const workItems = paths.size();
    Optional<std::vector<Ref<RepoOptionsFlags>>> storedOptions;

    using ExecT = decltype(g_indexJob)::ExecT;
    auto const doExec = [&] (
        auto configRef, auto metasRef, auto fileRefs, auto metadata
    ) -> coro::Task<ExecT> {
      auto out = co_await m_client.exec(
        g_indexJob,
        std::make_tuple(*configRef, std::move(metasRef)),
        std::move(fileRefs),
        std::move(metadata)
      );
      co_return out;
    };

    using IndexInputs = std::tuple<
      Ref<std::string>,
      Ref<RepoOptionsFlags>
    >;

    auto [declsRefs, summariesRef] = co_await coro::co_invoke(
      [&] () -> coro::Task<ExecT> {
        // Try optimistic mode first. We won't actually store
        // anything, just generate the Refs. If something isn't
        // actually present in the workers, the execution will throw
        // an exception. If everything is present, we've skipped a
        // lot of work.
        bool optimistic = Option::ParserOptimisticStore &&
                          m_client.supportsOptimistic();
        for (;;) {
          assertx(!metas.empty());
          Client::ExecMetadata metadata{
            .optimistic = optimistic,
            .job_key = folly::sformat("index-{}", metas[0].m_filename)
          };
          auto [configRef, metasRef, fileRefs, optionRefs] =
            co_await storeInputs(
              optimistic, paths, metas, options, storedOptions, m_client,
              m_config);
          // "Tuplize" the input refs according to signature of IndexJob::run()
          std::vector<IndexInputs> inputs;
          inputs.reserve(workItems);
          for (size_t i = 0; i < workItems; ++i) {
            inputs.emplace_back(
              std::move(fileRefs[i]),
              std::move(optionRefs[i])
            );
          }
          try {
            co_return co_await doExec(
              configRef, std::move(metasRef),
              std::move(inputs), std::move(metadata)
            );
          } catch (const extern_worker::WorkerError&) {
            throw;
          } catch (const extern_worker::Error&) {
            if (!optimistic) throw;
            optimistic = false;
          }
        }
      }
    );

    // Load the summaries but leave decls in external storage
    auto summaries = co_await m_client.load(summariesRef);
    assertx(metas.size() == workItems);
    assertx(declsRefs.size() == workItems);
    always_assert(summaries.size() == workItems);
    m_total += workItems;

    // Process the summaries
    for (size_t i = 0; i < workItems; ++i) {
      auto& meta = metas[i];
      auto& summary = summaries[i];
      auto& declsRef = declsRefs[i];
      if (summary.error.empty()) {
        callback(
            std::move(meta.m_filename),
            std::move(summary),
            std::move(declsRef)
        );
      } else {
        // Could not parse decls in this file. Compiler may fail, or produce
        // a unit that fatals when run.
        Logger::FWarning("Warning: decl-parser error in {}: {}",
            meta.m_filename, summary.error
        );
      }
    }
    co_return;
  } catch (const Exception& e) {
    Logger::FError(
      "Fatal: An unexpected exception was thrown while indexing: {}",
      e.getMessage()
    );
    m_failed.store(true);
  } catch (const Error& e) {
    Logger::FError("Extern worker error while indexing: {}",
                   e.what());
    m_failed.store(true);
  } catch (const std::exception& e) {
    Logger::FError(
      "Fatal: An unexpected exception was thrown while indexing: {}",
      e.what()
    );
    m_failed.store(true);
  } catch (...) {
    Logger::Error("Fatal: An unexpected exception was thrown while indexing");
    m_failed.store(true);
  }
  co_return;
}

///////////////////////////////////////////////////////////////////////////////

bool UnitIndex::containsAnyMissing(
  const Package::ParseMetaVec& parseMetas
) const {
  auto any = [&](auto const& names, auto const& map) -> bool {
    for (auto name : names) {
      if (map.find(name) != map.end()) return true;
    }
    return false;
  };
  for (auto const& p : parseMetas) {
    if (any(p.m_missing.types, types) ||
        any(p.m_missing.funcs, funcs) ||
        any(p.m_missing.constants, constants) ||
        any(p.m_missing.modules, modules)) {
      return true;
    }
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////

coro::Task<bool> Package::emit(const UnitIndex& index,
                               const EmitCallback& callback,
                               const LocalCallback& localCallback,
                               const std::filesystem::path& edgesPath,
                               const std::filesystem::path& filesInBuildPath,
                               bool logPackageBuildDrift) {
  assertx(callback);
  assertx(localCallback);

  Logger::FInfo(
    "emitting using {} threads using {}",
    m_executor.numThreads(),
    m_client.implName()
  );

  HphpSession _{Treadmill::SessionKind::CompilerEmit};

  // Treat any symbol refs from systemlib as if they were part of the
  // original Package. This allows systemlib to depend on input files
  // and parse-on-demand files.
  UEVec localUEs;
  for (auto& ue : SystemLib::claimRegisteredUnitEmitters()) {
    OndemandInfo ondemand;
    resolveOnDemand(ondemand, ue->m_filepath, ue->m_symbol_refs, index, true);
    for (auto const& p : ondemand.m_files) {
      addSourceFile(p.m_path);
    }
    for (auto const& e : ondemand.m_edges) {
      auto const sym = e.sym ? e.sym : makeStaticString("<include>");
      Logger::FVerbose("systemlib unit {} -> {} -> {}", e.from, sym, e.to);
    }
    localUEs.emplace_back(std::move(ue));
  }

  std::vector<coro::TaskWithExecutor<void>> tasks;
  if (!localUEs.empty()) {
    auto task = localCallback(std::move(localUEs));
    tasks.emplace_back(co_withExecutor(m_executor.sticky(), std::move(task)));
  }
  if (Cfg::Eval::PackageV2) {
    tasks.emplace_back(
      co_withExecutor(m_executor.sticky(),
        emitAllPackageV2(callback, index, edgesPath, filesInBuildPath, logPackageBuildDrift))
    );
  } else {
    tasks.emplace_back(
      co_withExecutor(m_executor.sticky(), emitAll(callback, index, edgesPath, filesInBuildPath))
    );
  }
  co_await coro::collectAllRange(std::move(tasks));
  co_return !m_failed.load();
}

namespace {
// Write every ondemand edge to a text file. The file contains
// enough information to reconstruct a file dependency graph with
// edges labeled with symbol names.
//
// Format:
//   f <filename>         Defines a file (node)
//   s <symbol>           Defines a symbol (edge label)
//   e <from> <to> <sym>  Dependency edge
//
// Edges reference files and symbols by index, where indexes are
// assigned in the order of `f` and `s` lines in the file.
// For example:
//
// a.php
//   function foo() {}
// b.php
//   function test() { foo(); }
//
// reports one edge:
//   f a.php
//   f b.php
//   s foo
//   e 0 1 0
//
void saveSymbolRefEdges(std::vector<SymbolRefEdge> edges,
                        const std::filesystem::path& edgesPath) {
  auto f = fopen(edgesPath.native().c_str(), "w");
  if (!f) {
    Logger::FError("Could not open {} for writing", edgesPath.native());
    return;
  }
  SCOPE_EXIT { fclose(f); };
  hphp_fast_map<const StringData*, size_t> files;
  hphp_fast_map<const StringData*, size_t> symbols;
  for (auto const& e : edges) {
    auto it = files.find(e.from);
    if (it == files.end()) {
      files.insert(std::make_pair(e.from, files.size()));
      fprintf(f, "f %s\n", e.from->data());
    }
    it = files.find(e.to);
    if (it == files.end()) {
      files.insert(std::make_pair(e.to, files.size()));
      fprintf(f, "f %s\n", e.to->data());
    }
    it = symbols.find(e.sym);
    if (it == symbols.end()) {
      symbols.insert(std::make_pair(e.sym, symbols.size()));
      auto const sym = e.sym ? e.sym->data() : "<include>";
      fprintf(f, "s %s\n", sym);
    }
    fprintf(f, "e %lu %lu %lu\n", files[e.from], files[e.to], symbols[e.sym]);
  }
  Logger::FInfo("Saved ondemand edges to {}", edgesPath.native());
}

const char* show(FileInBuildReason r) {
  switch (r) {
    case FileInBuildReason::fromIncludeDir:
      return "fromIncludedDir";
    case FileInBuildReason::fromSymbolRefs:
      return "fromSymbolRefs";
    case FileInBuildReason::fromPackageV1:
      return "fromPackageV1";
    case FileInBuildReason::fromPackageV2:
      return "fromPackageV2";
    case FileInBuildReason::fromSymbolRefsAndPackageV2:
      return "fromSymbolRefsAndPackageV2";
    case FileInBuildReason::notIncluded:
      return "notIncluded";
    case FileInBuildReason::noFilepath:
      return "noFilepath";
    case FileInBuildReason::unknownFile:
      return "unknownFile";
  }
}

void saveFilesInBuild(const std::vector<FileInBuildInfo>& files_in_build,
                      const std::filesystem::path& filesInBuildPath) {
  auto f = fopen(filesInBuildPath.native().c_str(), "w");
  if (!f) {
    Logger::FError("Could not open {} for writing", filesInBuildPath.native());
    return;
  }
  SCOPE_EXIT { fclose(f); };
  for (auto const& file : files_in_build) {
    fprintf(f, "f [%s] %s\n", show(file.m_reason), file.m_file->data());
  }
  Logger::FInfo("Saved the files in the build to {}", filesInBuildPath.native());
}

/* log to scuba files drifting between symbolrefs and package builds */
void logPackageDrift(const std::vector<FileInBuildInfo>& files_in_build) {
  StructuredLogEntry ent;
  for (auto const& file : files_in_build) {
    if (file.m_reason != FileInBuildReason::fromSymbolRefsAndPackageV2) {
      ent.setStr("filepath", file.m_file->data());
      ent.setStr("reason", show(file.m_reason));
      ent.force_init = true;
      StructuredLog::log("packagev2_www_build_drift", ent);
    }
  }
}

void saveBuildInfo(const std::vector<SymbolRefEdge>& edges,
                   const std::filesystem::path& edgesPath,
                   const std::vector<FileInBuildInfo>& files_in_build,
                   const std::filesystem::path& filesInBuildPath,
                   bool logPackageBuildDrift) {
  if (!edgesPath.native().empty()) {
    saveSymbolRefEdges(std::move(edges), edgesPath);
  }
  if (!filesInBuildPath.native().empty()) {
    saveFilesInBuild(files_in_build, filesInBuildPath);
  }
  if (logPackageBuildDrift) {
    logPackageDrift(files_in_build);
  }
}
}

// The actual emit loop for SymbolRefs and PackageV1 builds.
// Find the initial set of inputs (from configuration), emit them,
// enumerate on-demand files from symbol refs,
// then repeat the process until we have no new files to emit.
coro::Task<void>
Package::emitAll(const EmitCallback& callback, const UnitIndex& index,
                 const std::filesystem::path& edgesPath,
                 const std::filesystem::path& filesInBuildPath) {
  auto const logEdges = !edgesPath.native().empty();
  auto const logFiles = !filesInBuildPath.native().empty();

  // Find the initial set of groups
  auto input_groups = co_await groupAll(m_directories, true, true);

  // Select the files specified as inputs, collect ondemand file names.
  std::vector<SymbolRefEdge> edges;
  std::vector<FileInBuildInfo> files_in_build;
  EmitInfo info;

  {
    Timer timer{Timer::WallTime, "emitting inputs"};
    info = co_await emitGroups(
      std::move(input_groups), callback, index, EmitPass::IncludeDirFiles, logFiles);
    if (logEdges) {
      edges.insert(
        edges.end(), info.m_ondemand.m_edges.begin(), info.m_ondemand.m_edges.end()
      );
    }
    if (logFiles) {
      files_in_build.insert(
        files_in_build.end(), info.m_files_in_build.begin(), info.m_files_in_build.end()
      );
    }
    m_inputMicros = std::chrono::microseconds{timer.getMicroSeconds()};
  }

  if (info.m_ondemand.m_files.empty()) {
    if (logFiles) {
      saveFilesInBuild(files_in_build, filesInBuildPath);
    }
    co_return;
  }

  Timer timer{Timer::WallTime, "emitting on-demand"};
  // We have ondemand files, so keep emitting until a fix point.
  do {
    Groups ondemand_groups;
    groupFiles(ondemand_groups, std::move(info.m_ondemand.m_files));
    info = co_await emitGroups(
      std::move(ondemand_groups), callback, index, EmitPass::SymbolRefsOnDemandFiles, logFiles);
    if (logEdges) {
      edges.insert(
        edges.end(), info.m_ondemand.m_edges.begin(), info.m_ondemand.m_edges.end()
      );
    }
    if (logFiles) {
      files_in_build.insert(
        files_in_build.end(), info.m_files_in_build.begin(), info.m_files_in_build.end()
      );
    }
  } while (!info.m_ondemand.m_files.empty());
  m_ondemandMicros = std::chrono::microseconds{timer.getMicroSeconds()};

  // Save edge and file list to a text file, if requested
  if (logEdges) {
    saveSymbolRefEdges(std::move(edges), edgesPath);
  }
  if (logFiles) {
    saveFilesInBuild(files_in_build, filesInBuildPath);
  }

  co_return;
}

void Package::logBuildInfo(std::vector<SymbolRefEdge>& edges,
                           std::vector<FileInBuildInfo>& files_in_build,
                           const Package::EmitInfo& info,
                           bool logEdges, bool logFiles) {
  if (logEdges) {
    edges.insert(
      edges.end(), info.m_ondemand.m_edges.begin(), info.m_ondemand.m_edges.end()
    );
  }
  if (logFiles) {
    files_in_build.insert(
      files_in_build.end(), info.m_files_in_build.begin(), info.m_files_in_build.end()
    );
  }
}

// The actual emit loop once PackageV2 are enable.  It supports building with
// - SymbolRefs only
// - PackageV2 only
// - SymbolRefs U Package2
// It follows the overall strategy of emitAll, but it ensures that PackageV2
// builds include all files with __PackageOverride annotations in directories
// excluded with --exclude-dirs (eg. files pulled into prod from flib/intern).
// Files excluded with --exclude-file and --exclude-pattern are never included
// in the build (eg files in __tests__/ direcories).
coro::Task<void>
Package::emitAllPackageV2(const EmitCallback& callback, const UnitIndex& index,
                 const std::filesystem::path& edgesPath,
                 const std::filesystem::path& filesInBuildPath,
                 bool logPackageBuildDrift) {
  auto const logEdges = !edgesPath.native().empty();
  auto const logFiles =
    !filesInBuildPath.native().empty() || logPackageBuildDrift;

  // Find the initial set of groups
  // - if building with SymbolRefs, both files and dirs must be filtered out
  // - if building with PackagesV2, both files and dirs must be included
  // - if building with PackagesV2 U SymbolRefs,
  //   - both files and dirs must be initially filtered out;
  //     - files and dirs that have been filtered out are stored in `filtered`
  //   - symbolrefs is run
  //   - emit is then invoked on ignored files, pulling in files according to
  //     package rules
  //   - if a file is pulled both by symbolrefs and packages, the duplicate
  //     is discarded by the `emitRemoteUnit` callback

  assertx(Cfg::Eval::PackageV2);
  const PackageV2BuildMode buildMode = packageV2BuildMode();
  assertx(buildMode != PackageV2BuildMode::packageV2Disabled);

  // Emit files specified as inputs, collect ondemand file names.
  // - If building with PackageV2 only, then --excluded-dirs are not
  //   skipped to do the build in one pass.
  // - If building with SymbolRefs, then Package check are skipped as
  //   all SymbolRefs files must be in the build.
  auto input_groups =
    co_await groupAll(m_directories, true, isSymbolRefsEnabled(buildMode));

  std::vector<SymbolRefEdge> edges;
  std::vector<FileInBuildInfo> files_in_build;
  EmitInfo info;

  {
    Timer timer{Timer::WallTime, "emitting inputs"};
    info = co_await emitGroups(
      std::move(input_groups), callback, index,
      buildMode == PackageV2BuildMode::packagesOnly ?
        EmitPass::PackageV2RulesOnly : EmitPass::IncludeDirFiles,
      logFiles);
    logBuildInfo(edges, files_in_build, info, logEdges, logFiles);
    m_inputMicros = std::chrono::microseconds{timer.getMicroSeconds()};
  }

  // Emit is completed if
  // - building with SymbolRefs only and there no ondemand files, or
  // - building with PackageV2 only
  if ((info.m_ondemand.m_files.empty() &&
       buildMode == PackageV2BuildMode::symbolRefsOnly) ||
      buildMode == PackageV2BuildMode::packagesOnly) {

    // Before returning, save build information to text files and/or scuba
    saveBuildInfo(std::move(edges), edgesPath,
                  std::move(files_in_build), filesInBuildPath,
                  logPackageBuildDrift);

    co_return;
  }

  // Emit ondemand files
  // Files pulled in by SymbolRefs are NOT filtered by PackageV2 rules
  if (!info.m_ondemand.m_files.empty()) {
    Timer timer{Timer::WallTime, "emitting on-demand"};
    // We have ondemand files, so keep emitting until a fix point.
    do {
      Groups ondemand_groups;
      groupFiles(ondemand_groups, std::move(info.m_ondemand.m_files));
      info = co_await emitGroups(
        std::move(ondemand_groups), callback, index,
        EmitPass::SymbolRefsOnDemandFiles,
        logFiles);
      logBuildInfo(edges, files_in_build, info, logEdges, logFiles);
    } while (!info.m_ondemand.m_files.empty());
    m_ondemandMicros = std::chrono::microseconds{timer.getMicroSeconds()};
  }

  // When building with an ActiveDeployement and ForceEnableSymbolRefs
  // emit files with __PackageOverride annotations living in directories
  // skipped with --exclude-dirs.  This step is not needed if building
  // only with Packages, as in that case excluded-dirs were handled in
  // the first pass
  if (buildMode == PackageV2BuildMode::packagesAndSymbolRefs) {
    Timer timer{Timer::WallTime, "emitting filtered files included by package rules"};

    // Excluded dirs must be scanned for __PackageOverride only if they are
    // subdirectories of directories explicitly included in the build with --dir
    std::set<std::string> excluded_dirs;
    for (auto const& e : Option::PackageExcludeDirs) {
      if (std::any_of(m_directories.begin(), m_directories.end(),
          [e](const auto& i){ return e.starts_with(i); } )) {
        excluded_dirs.emplace(e);
      } else {
        Logger::FWarning(
          "The excluded directory {} is not a subdirectory of a directory included in the build,"
          " It is not scanned for __PackageOverride annotations.",
          e);
      }
    }

    // excluded dirs might not exist at all: do not fail hard
    Groups filtered_groups =
      co_await groupAll(excluded_dirs, true, false, /* failHard */ false);
    info = co_await emitGroups(
      std::move(filtered_groups), callback, index,
      EmitPass::PackageV2RulesOnly,
      logFiles);
    m_filteredPackageMicros = std::chrono::microseconds{timer.getMicroSeconds()};
    logBuildInfo(edges, files_in_build, info, false, logFiles);
  }

  // Before returning, save build information to text files and/or scuba
  saveBuildInfo(std::move(edges), edgesPath,
                std::move(files_in_build), filesInBuildPath,
                logPackageBuildDrift);

  co_return;
}

// Emit all of the files in the given group, returning a vector of
// ondemand files obtained from that group.
coro::Task<Package::EmitInfo> Package::emitGroups(
  Groups groups,
  const EmitCallback& callback,
  const UnitIndex& index,
  EmitPass emitPass,
  bool logFiles
) {
  if (groups.empty()) co_return EmitInfo{};

  // Kick off emitting. Each group gets its own sticky ticket (so
  // earlier groups will get scheduling priority over later ones).
  std::vector<coro::TaskWithExecutor<EmitInfo>> tasks;
  for (auto& group : groups) {
    tasks.emplace_back(
		  co_withExecutor(m_executor.sticky(),
        emitGroup(std::move(group), callback, index, emitPass, logFiles))
    );
  }

  // Gather the on-demand files and emitted files and return them
  OndemandInfo ondemand;
  std::vector<FileInBuildInfo> files_in_build;
  for (auto& info : co_await coro::collectAllRange(std::move(tasks))) {
    ondemand.m_files.insert(
      ondemand.m_files.end(),
      std::make_move_iterator(info.m_ondemand.m_files.begin()),
      std::make_move_iterator(info.m_ondemand.m_files.end())
    );
    ondemand.m_edges.insert(
      ondemand.m_edges.end(),
      std::make_move_iterator(info.m_ondemand.m_edges.begin()),
      std::make_move_iterator(info.m_ondemand.m_edges.end())
    );
    files_in_build.insert(
      files_in_build.end(),
      std::make_move_iterator(info.m_files_in_build.begin()),
      std::make_move_iterator(info.m_files_in_build.end())
    );
  }
  co_return EmitInfo{std::move(ondemand), std::move(files_in_build)};
}

// Emit a group, hand off the UnitEmitter or WPI::Key/Value obtained,
// and return any on-demand files found.
coro::Task<Package::EmitInfo> Package::emitGroup(
    Group group,
    const EmitCallback& callback,
    const UnitIndex& index,
    EmitPass emitPass,
    bool logFiles
) {
  using namespace folly::gen;

  // Make sure we're running on the thread we should be
  co_await coro::co_reschedule_on_current_executor;

  try {
    if (group.m_files.empty()) co_return EmitInfo{};

    auto const workItems = group.m_files.size();
    for (size_t i = 0; i < workItems; i++) {
      auto const& fileName = group.m_files[i];
      if (!m_extraStaticFiles.contains(fileName)) {
        m_discoveredStaticFiles.emplace(
          fileName,
          Option::CachePHPFile ? createFullPath(fileName, m_root) : ""
        );
      }
    }

    // The callback takes a group of files and returns the parse metas
    // associated with these files. During the emit process, the units that
    // do not belong to the active deployment will not produce parse metas
    // as they do not need to be part of the final repo product.
    // This will mean that the size of the parse metas does not need to be
    // equal to number of files sent to the callback.
    // One problem this creates is that we need to be able to associate
    // parse metas with original group order. In order to do this,
    // we also return a fixup list consisting of original indicies of the
    // omitted units. We later use this fixup list to compute the original
    // indicies of each unit.
    auto parseMetasReasonsAndItemsToSkip =
      co_await callback(group.m_files, emitPass);
    auto& [parseMetas, reasons, itemsToSkip] = parseMetasReasonsAndItemsToSkip;
    if (Cfg::Eval::ActiveDeployment.empty()) {
      // If a deployment is not set, then we should have gotten results for
      // all files
      always_assert(parseMetas.size() == workItems);
    }
    m_total += parseMetas.size();

    // Process the outputs
    OndemandInfo ondemand;
    std::vector<FileInBuildInfo> files_in_build;
    size_t numSkipped = 0;
    for (size_t i = 0; i < workItems; i++) {
      if (itemsToSkip.contains(i)) {
        numSkipped++;
        continue;
      }
      auto const& meta = parseMetas[i - numSkipped];
      if (!meta.m_abort.empty()) {
        // The unit had an ICE and we're configured to treat that as a
        // fatal error. Here is where we die on it.
        fprintf(stderr, "%s", meta.m_abort.c_str());
        _Exit(HPHP_EXIT_FAILURE);
      }
      auto const filename = makeStaticString(group.m_files[i].native());
      if (logFiles) {
        files_in_build.push_back(FileInBuildInfo{filename, reasons[i]});
      }
      if (Option::ForceEnableSymbolRefs || Cfg::Eval::ActiveDeployment.empty()) {
        // Resolve any symbol refs into files to parse ondemand
        resolveOnDemand(ondemand, filename, meta.m_symbol_refs, index);
      }
    }
    co_return EmitInfo{std::move(ondemand), std::move(files_in_build)};
  } catch (const Exception& e) {
    Logger::FError(
      "Fatal: An unexpected exception was thrown while emitting: {}",
      e.getMessage()
    );
    m_failed.store(true);
  } catch (const std::exception& e) {
    Logger::FError(
      "Fatal: An unexpected exception was thrown while emitting: {}",
      e.what()
    );
    m_failed.store(true);
  } catch (...) {
    Logger::Error("Fatal: An unexpected exception was thrown while emitting");
    m_failed.store(true);
  }

  co_return EmitInfo{};
}

Package::IndexMeta HPHP::summary_of_symbols(const hackc::FileSymbols& symbols) {
  Package::IndexMeta summary;
  for (auto& e : symbols.types) {
    summary.types.emplace_back(makeStaticString(std::string(e)));
  }
  for (auto& e : symbols.functions) {
    summary.funcs.emplace_back(makeStaticString(std::string(e)));
  }
  for (auto& e : symbols.constants) {
    summary.constants.emplace_back(makeStaticString(std::string(e)));
  }
  for (auto& e : symbols.modules) {
    summary.modules.emplace_back(makeStaticString(std::string(e)));
  }
  return summary;
}
