include
  (val if Injector_config.use_test_stubbing then
         (module Test_disk : Disk_sig.S)
       else
         (module Real_disk : Disk_sig.S))
