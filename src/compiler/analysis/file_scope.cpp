/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010 Facebook, Inc. (http://www.facebook.com)          |
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

#include <compiler/analysis/file_scope.h>
#include <compiler/analysis/code_error.h>
#include <compiler/analysis/analysis_result.h>
#include <compiler/analysis/class_scope.h>
#include <compiler/statement/statement_list.h>
#include <compiler/statement/exp_statement.h>
#include <compiler/option.h>
#include <compiler/analysis/constant_table.h>
#include <compiler/analysis/function_scope.h>
#include <sys/stat.h>
#include <compiler/parser/parser.h>
#include <util/logger.h>
#include <util/util.h>
#include <compiler/expression/expression_list.h>
#include <compiler/statement/function_statement.h>
#include <compiler/analysis/variable_table.h>
#include <compiler/expression/simple_function_call.h>

using namespace HPHP;
using namespace boost;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

FileScope::FileScope(const string &fileName, int fileSize)
  : BlockScope("", "", StatementPtr(), BlockScope::FileScope),
    m_size(fileSize), m_fileName(fileName) {
  pushAttribute(); // for global scope
}

///////////////////////////////////////////////////////////////////////////////
// parser functions

FunctionScopePtr FileScope::setTree(AnalysisResultPtr ar,
                                    StatementListPtr tree) {
  m_tree = tree;

  for (int i = 0; i < tree->getCount(); i++) {
    StatementPtr stmt = (*tree)[i];
    stmt->setFileLevel();
    if (stmt->is(Statement::KindOfExpStatement)) {
      ExpStatementPtr expStmt = dynamic_pointer_cast<ExpStatement>(stmt);
      ExpressionPtr exp = expStmt->getExpression();
      exp->setFileLevel();
    }
  }
  return createPseudoMain(ar);
}

bool FileScope::addClass(AnalysisResultPtr ar, ClassScopePtr classScope) {
  if (ar->declareClass(classScope)) {
    m_classes[classScope->getName()].push_back(classScope);
    return true;
  }
  m_ignoredClasses.push_back(classScope);
  return false;
}

ClassScopePtr FileScope::getClass(const char *name) {
  StringToClassScopePtrVecMap::const_iterator iter = m_classes.find(name);
  if (iter == m_classes.end()) return ClassScopePtr();
  return iter->second.back();
}


int FileScope::getFunctionCount() const {
  int total = FunctionContainer::getFunctionCount();
  for (StringToClassScopePtrVecMap::const_iterator iter = m_classes.begin();
       iter != m_classes.end(); ++iter) {
    BOOST_FOREACH(ClassScopePtr cls, iter->second) {
      total += cls->getFunctionCount();
    }
  }
  return total;
}

void FileScope::countReturnTypes(std::map<std::string, int> &counts) {
  FunctionContainer::countReturnTypes(counts);
  for (StringToClassScopePtrVecMap::const_iterator iter = m_classes.begin();
       iter != m_classes.end(); ++iter) {
    BOOST_FOREACH(ClassScopePtr cls, iter->second) {
      cls->countReturnTypes(counts);
    }
  }
}

void FileScope::pushAttribute() {
  m_attributes.push_back(0);
}

void FileScope::setAttribute(Attribute attr) {
  ASSERT(!m_attributes.empty());
  m_attributes.back() |= attr;
}

int FileScope::popAttribute() {
  ASSERT(!m_attributes.empty());
  int ret = m_attributes.back();
  m_attributes.pop_back();
  return ret;
}

int FileScope::getGlobalAttribute() const {
  ASSERT(m_attributes.size() == 1);
  return m_attributes.back();
}

///////////////////////////////////////////////////////////////////////////////

ExpressionPtr FileScope::getEffectiveImpl(AnalysisResultPtr ar) const {
  if (m_tree) return m_tree->getEffectiveImpl(ar);
  return ExpressionPtr();
}

bool FileScope::hasImpl(AnalysisResultPtr ar) const {
  return m_tree && m_tree->hasImpl();
}

///////////////////////////////////////////////////////////////////////////////

void FileScope::declareConstant(AnalysisResultPtr ar, const string &name) {
  if (!ar->declareConst(shared_from_this(), name)) {
    addConstantDependency(ar, name);
  }
}

void FileScope::addConstant(const string &name, TypePtr type,
                            ExpressionPtr value,
                            AnalysisResultPtr ar, ConstructPtr con) {
  BlockScopePtr f = ar->findConstantDeclarer(name);
  cout << "Add constant " << name << " in " << f->getName() << endl;
  f->getConstants()->add(name, type, value, ar, con);
}

void FileScope::addIncludeDependency(AnalysisResultPtr ar,
                                     const string &file, bool byInlined) {
  ar->addIncludeDependency(shared_from_this(), file);
  if (byInlined) m_usedIncludesInline.insert(file);
}
void FileScope::addClassDependency(AnalysisResultPtr ar,
                                   const string &classname) {
  if (m_usedClasses.find(classname) == m_usedClasses.end()) {
    m_usedClasses.insert(classname);
    ar->addClassDependency(shared_from_this(), classname);
  }
}
void FileScope::addFunctionDependency(AnalysisResultPtr ar,
                                      const string &funcname, bool byInlined) {
  ar->addFunctionDependency(shared_from_this(), funcname);
  if (byInlined) m_usedFuncsInline.insert(funcname);
}
void FileScope::addConstantDependency(AnalysisResultPtr ar,
                                      const string &decname) {
  if (m_usedConsts.find(decname) == m_usedConsts.end()) {
    m_usedConsts.insert(decname);
    ar->addConstantDependency(shared_from_this(), decname);
  }
}

void FileScope::analyzeProgram(AnalysisResultPtr ar) {
  if (m_pseudoMain) {
    m_pseudoMain->getStmt()->analyzeProgram(ar);
  }
}

void FileScope::visit(AnalysisResultPtr ar,
                      void (*cb)(AnalysisResultPtr, StatementPtr, void*),
                      void *data)
{
  if (m_pseudoMain) {
    cb(ar, m_pseudoMain->getStmt(), data);
  }
}

void FileScope::inferTypes(AnalysisResultPtr ar) {
  if (m_pseudoMain) {
    m_pseudoMain->getStmt()->inferTypes(ar);
  }
}

const string &FileScope::pseudoMainName() {
  if (m_pseudoMainName.empty()) {
    m_pseudoMainName = Option::MangleFilename(m_fileName, true);
  }
  return m_pseudoMainName;
}

FunctionScopePtr FileScope::createPseudoMain(AnalysisResultPtr ar) {
  StatementListPtr st = m_tree;
  FunctionStatementPtr f
    (new FunctionStatement(BlockScopePtr(), LocationPtr(),
                           Statement::KindOfFunctionStatement,
                           false, pseudoMainName(),
                           ExpressionListPtr(), st, 0, ""));
  f->setFileLevel();
  FunctionScopePtr pseudoMain(
    new HPHP::FunctionScope(ar, true,
                            pseudoMainName().c_str(),
                            f, false, 0, 0,
                            ModifierExpressionPtr(),
                            m_attributes[0], "",
                            shared_from_this(),
                            true));
  f->setBlockScope(pseudoMain);
  m_functions[pseudoMainName()].push_back(pseudoMain);
  m_pseudoMain = pseudoMain;
  return pseudoMain;
}

string FileScope::outputFilebase() {
  string file = m_fileName;
  string out;
  if (file.size() > 4 && file.substr(file.length() - 4) == ".php") {
    out = file.substr(0, file.length() - 4);
  } else {
    out = file + ".nophp";
  }
  return Option::MangleFilename(out, false);
}

void FileScope::outputCPPHelper(CodeGenerator &cg, AnalysisResultPtr ar,
                                bool classes /* = true */) {
  if (classes) {
    for (StringToClassScopePtrVecMap::iterator it = m_classes.begin();
         it != m_classes.end(); ++it) {
      BOOST_FOREACH(ClassScopePtr cls, it->second) {
        cls->getStmt()->outputCPP(cg, ar);
      }
    }
  }
  for (StringToFunctionScopePtrVecMap::iterator it = m_functions.begin();
       it != m_functions.end(); ++it) {
    BOOST_FOREACH(FunctionScopePtr func, it->second) {
      func->getStmt()->outputCPP(cg, ar);
    }
  }
}

void FileScope::outputCPPImpl(CodeGenerator &cg, AnalysisResultPtr ar) {
  ASSERT(cg.getContext() == CodeGenerator::CppImplementation);

  cg.setContext(CodeGenerator::CppConstantsDecl);
  getConstants()->outputCPP(cg, ar);

  cg.setContext(CodeGenerator::CppImplementation);
  cg_printf("/* preface starts */\n");
  for (vector<lambda>::const_iterator it = m_lambdas.begin();
       it != m_lambdas.end(); ++it) {
    cg_indentBegin("inline %s %s(%s) {\n", it->rt.c_str(), it->name.c_str(),
                   it->args.c_str());
    cg_printf("%s\n", it->body.c_str());
    cg_indentEnd("}\n");
  }

  for (StringToFunctionScopePtrVecMap::iterator it = m_functions.begin();
       it != m_functions.end(); ++it) {
    BOOST_FOREACH(FunctionScopePtr func, it->second) {
      cg_printf("extern CallInfo %s%s;\n", Option::CallInfoPrefix,
                func->getId(cg).c_str());
    }
  }

  cg_printf("/* preface finishes */\n");

  outputCPPHelper(cg, ar);

  if (Option::GenerateCPPMacros) {
    bool hasRedec;
    outputCPPJumpTableSupport(cg, ar, hasRedec);
    outputCPPCallInfoTableSupport(cg, ar, hasRedec);
    for (StringToClassScopePtrVecMap::iterator it = m_classes.begin();
         it != m_classes.end(); ++it) {
      BOOST_FOREACH(ClassScopePtr cls, it->second) {
        if (!cls->isInterface()) {
          cls->outputCPPDynamicClassImpl(cg, ar);
        }
      }
    }
  }
}

void FileScope::outputCPPPseudoMain(CodeGenerator &cg, AnalysisResultPtr ar) {
  ASSERT(cg.getContext() == CodeGenerator::CppPseudoMain);
  outputCPPHelper(cg, ar, false);
}

void FileScope::outputCPPForwardDeclarations(CodeGenerator &cg,
                                             AnalysisResultPtr ar) {
  cg.printSection("Forward Declarations");
  cg.setContext(CodeGenerator::CppForwardDeclaration);

  string name;
  ClassScopePtr cls;

  map<string, FileScopePtr> extraIncs;
  BOOST_FOREACH(name, m_usedClasses) {
    cls = ar->findClass(name, AnalysisResult::ClassName);
    if (cls && cls->isUserClass()) {
      FileScopePtr fs = cls->getContainingFile();
      if (fs) {
        extraIncs[fs->getName()] = fs;
      }
    }
  }
  BOOST_FOREACH(name, m_usedConsts) {
    BlockScopePtr block = ar->findConstantDeclarer(name);
    if (block && block->is(BlockScope::FileScope)) {
      FileScopePtr fs = dynamic_pointer_cast<FileScope>(block);
      extraIncs[fs->getName()] = fs;
    }
  }

  cg.namespaceBegin();
  cg.printSection("1. Static Strings", false);
  string str;
  BOOST_FOREACH(str, m_usedLiteralStringsHeader) {
    int index = -1;
    int stringId = cg.checkLiteralString(str, index, ar, BlockScopePtr());
    assert(index != -1);
    string lisnam = ar->getLiteralStringName(stringId, index);
    cg_printf("extern StaticString %s;\n", lisnam.c_str());
  }
  cg_printf("\n");
  cg.printSection("2. Static Arrays", false);
  BOOST_FOREACH(str, m_usedDefaultValueScalarArrays) {
    int index = -1;
    int hash = ar->checkScalarArray(str, index);
    assert(hash != -1 && index != -1);
    string name = ar->getScalarArrayName(hash, index);
    cg_printf("extern StaticArray %s;\n", name.c_str());
  }
  cg_printf("\n");
  cg.printSection("3. Constants", false);
  if (cg.getOutput() != CodeGenerator::MonoCPP) {
    getConstants()->outputCPP(cg, ar);
  } else {
    cg_printf("// (omitted in MonoCPP mode)\n");
  }

  cg.printSection("4. Classes");
  for (StringToClassScopePtrVecMap::iterator it = m_classes.begin();
       it != m_classes.end(); ++it) {
    BOOST_FOREACH(ClassScopePtr cls, it->second) {
      cls->getStmt()->outputCPP(cg, ar);
    }
  }

  cg.namespaceEnd();
  // Includes must come after classes and constants
  for (map<string, FileScopePtr>::const_iterator iter = extraIncs.begin();
       iter != extraIncs.end(); ++iter) {
    FileScopePtr fs = iter->second;
    if (fs != shared_from_this()) {
      cg_printInclude(fs->outputFilebase() + ".fw.h");
    }
  }
}

void FileScope::outputCPPDeclarations(CodeGenerator &cg,
                                      AnalysisResultPtr ar) {
  cg.printSection("Declarations");
  cg.setContext(CodeGenerator::CppDeclaration);

  if (cg.getOutput() == CodeGenerator::MonoCPP) {
    cg.namespaceBegin();
    outputCPPHelper(cg, ar);
    cg.namespaceEnd();
  } else {
    set<FileScopePtr> done;
    done.insert(shared_from_this());

    // Class declarations cunningly expressed as includes so I don't
    // have to worry about inheritance dependencies.
    for (StringToClassScopePtrVecMap::iterator it = m_classes.begin();
         it != m_classes.end(); ++it) {
      BOOST_FOREACH(ClassScopePtr cls, it->second) {
        cg_printInclude(cls->getHeaderFilename(cg));
      }
    }

    BOOST_FOREACH(string name, m_usedClasses) {
      ClassScopePtr cls = ar->findClass(name, AnalysisResult::ClassName);
      if (cls && cls->isUserClass()) {
        FileScopePtr fs = cls->getContainingFile();
        if (fs && done.find(fs) == done.end()) {
          done.insert(fs);
          cg_printInclude(fs->outputFilebase());
        }
      }
    }

    BOOST_FOREACH(string name, m_usedFuncsInline) {
      FunctionScopePtr func = ar->findFunction(name);
      if (func) {
        FileScopePtr fs = func->getContainingFile();
        if (fs && done.find(fs) == done.end()) {
          done.insert(fs);
          cg_printInclude(fs->outputFilebase());
        }
      }
    }
    BOOST_FOREACH(string name, m_usedIncludesInline) {
      FileScopePtr fs = ar->findFileScope(name);
      if (fs && done.find(fs) == done.end()) {
        done.insert(fs);
        cg_printInclude(fs->outputFilebase());
      }
    }

    cg.namespaceBegin();
    cg.setContext(CodeGenerator::CppForwardDeclaration);
    cg.printSection("Includes and Functions", false);
    outputCPPHelper(cg, ar, false); // function forward declarations
    cg.setContext(CodeGenerator::CppDeclaration);
    outputCPPHelper(cg, ar, false); // function declarations (only inline)

    cg.printSection("Redeclared Functions");
    outputCPPJumpTableDecl(cg, ar);

    cg.printSection("Dynamic Class Declarations");
    for (StringToClassScopePtrVecMap::iterator it = m_classes.begin();
         it != m_classes.end(); ++it) {
      BOOST_FOREACH(ClassScopePtr cls, it->second) {
        if (!cls->isInterface()) {
          cls->outputCPPDynamicClassDecl(cg);
        }
      }
    }
    cg.namespaceEnd();
  }
}

void FileScope::outputCPPForwardDeclHeader(CodeGenerator &cg,
                                           AnalysisResultPtr ar) {
  string header = outputFilebase() + ".fw.h";
  cg.headerBegin(header);
  if (Option::GenerateCPPMain) {
    cg_printInclude("<runtime/base/hphp.h>");
    cg_printInclude(string(Option::SystemFilePrefix) +
                    "literal_strings_remap.h");
    cg_printInclude(string(Option::SystemFilePrefix) +
                    "scalar_arrays_remap.h");
    cg_printInclude(string(Option::SystemFilePrefix) + "global_variables.h");
    cg_printInclude(string(Option::SystemFilePrefix) + "cpputil.h");
  } else if (cg.getOutput() == CodeGenerator::SystemCPP) {
    cg_printInclude("<runtime/base/hphp_system.h>");
    cg_printInclude(string(Option::SystemFilePrefix) +
                    "literal_strings_remap.h");
    cg_printInclude(string(Option::SystemFilePrefix) +
                    "scalar_arrays_remap.h");
  }
  outputCPPForwardDeclarations(cg, ar);
  cg.headerEnd(header);
}

void FileScope::outputCPPForwardStaticDecl(CodeGenerator &cg,
                                           AnalysisResultPtr ar) {
  string header = outputFilebase() + ".fws.h";
  cg.headerBegin(header);
  cg.namespaceBegin();
  cg.printSection("1. Static Strings", false);
  string str;
  BOOST_FOREACH(str, m_usedLiteralStrings) {
    if (m_usedLiteralStringsHeader.find(str) !=
        m_usedLiteralStringsHeader.end()) {
      continue;
    }
    int index = -1;
    int stringId = cg.checkLiteralString(str, index, ar, BlockScopePtr());
    assert(index != -1);
    string lisnam = ar->getLiteralStringName(stringId, index);
    cg_printf("extern StaticString %s;\n", lisnam.c_str());
  }
  cg_printf("\n");
  cg.printSection("2. Static Arrays", false);
  BOOST_FOREACH(str, m_usedScalarArrays) {
    if (m_usedDefaultValueScalarArrays.find(str) !=
        m_usedDefaultValueScalarArrays.end()) {
      continue;
    }
    int index = -1;
    int hash = ar->checkScalarArray(str, index);
    assert(hash != -1 && index != -1);
    string name = ar->getScalarArrayName(hash, index);
    cg_printf("extern StaticArray %s;\n", name.c_str());
  }
  cg.namespaceEnd();
  cg_printf("\n");
  cg.headerEnd(header);
}

void FileScope::outputCPPDeclHeader(CodeGenerator &cg, AnalysisResultPtr ar) {
  string fwheader = outputFilebase() + ".fw.h";
  string header = outputFilebase() + ".h";
  cg.headerBegin(header);
  if (Option::GenerateCPPMain) {
    cg_printInclude("<runtime/base/hphp.h>");
    cg_printInclude(string(Option::SystemFilePrefix) + "global_variables.h");
    cg_printInclude(string(Option::SystemFilePrefix) + "cpputil.h");
  } else if (cg.getOutput() == CodeGenerator::SystemCPP) {
    cg_printInclude("<runtime/base/hphp_system.h>");
  }
  cg_printInclude(fwheader);
  outputCPPDeclarations(cg, ar);
  cg.headerEnd(header);
}

void FileScope::outputCPPClassHeaders(CodeGenerator &cg, AnalysisResultPtr ar,
                                      CodeGenerator::Output output) {
  string name;
  ClassScopePtr cls;
  for (StringToClassScopePtrVecMap::iterator it = m_classes.begin();
       it != m_classes.end(); ++it) {
    BOOST_FOREACH(ClassScopePtr cls, it->second) {
      cls->outputCPPHeader(cg, ar, output);
    }
  }
}

void FileScope::outputCPPFFI(CodeGenerator &cg,
                             AnalysisResultPtr ar) {
  cg.setContext(CodeGenerator::CppFFIDecl);
  cg.useStream(CodeGenerator::PrimaryStream);
  outputCPPHelper(cg, ar, true);
  cg.useStream(CodeGenerator::ImplFile);
  cg.setContext(CodeGenerator::CppFFIImpl);
  cg_printInclude(outputFilebase() + ".h");
  outputCPPHelper(cg, ar, true);
}

void FileScope::outputHSFFI(CodeGenerator &cg,
                            AnalysisResultPtr ar) {
  cg.setContext(CodeGenerator::HsFFI);
  outputCPPHelper(cg, ar, false);
}

void FileScope::outputJavaFFI(CodeGenerator &cg, AnalysisResultPtr ar) {
  // first, generate methods in HphpMain.java
  for (StringToFunctionScopePtrVecMap::iterator it = m_functions.begin();
       it != m_functions.end(); ++it) {
    BOOST_FOREACH(FunctionScopePtr func, it->second) {
      func->getStmt()->outputCPP(cg, ar);
    }
  }

  // for each php class or interface, generate a Java file
  for (StringToClassScopePtrVecMap::iterator it = m_classes.begin();
       it != m_classes.end(); ++it) {
    BOOST_FOREACH(ClassScopePtr cls, it->second) {
      cls->getStmt()->outputCPP(cg, ar);
    }
  }
}

void FileScope::outputJavaFFICPPStub(CodeGenerator &cg,
                                     AnalysisResultPtr ar) {
  if (cg.getContext() == CodeGenerator::JavaFFICppImpl) {
    // java_stubs.h doesn't need the extra include
    cg_printInclude(outputFilebase() + ".h");
  }
  outputCPPHelper(cg, ar, true);
}

void FileScope::outputSwigFFIStubs(CodeGenerator &cg, AnalysisResultPtr ar) {
  // currently only support toplevel functions
  outputCPPHelper(cg, ar, false);
}
