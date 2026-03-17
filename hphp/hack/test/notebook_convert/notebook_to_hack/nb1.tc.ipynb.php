<?hh
// copyright header here
//@bento-notebook:{"notebook_number":"N1234","kernelspec":{"display_name":"hack","language":"hack","name":"bento_kernel_hack"}}
//@bento-cell:{"cell_bento_metadata":{"collapsed":true,"output":{"id":1247934846418027,"loadingStatus":"loaded"}},"id":1,"cell_type":"code"}
/* Very classy */
class N1234MyClass {}
//@bento-cell-end

//@bento-cell:{"cell_bento_metadata":{},"id":3,"cell_type":"markdown"}
/*@non_hack:
## Markdown cell (cell 3)

I am a *markdown* **cell**
*/
//@bento-cell-end

async function gen_n1234_notebook_main(): Awaitable<void> {
  //@bento-cell:{"cell_bento_metadata":{"collapsed":true,"output":{"id":1247934846418027,"loadingStatus":"loaded"}},"id":1,"cell_type":"code"}
  echo "hi from cell 1";
  //@bento-cell-end
  //@bento-cell:{"cell_bento_metadata":{"collapsed":true,"output":{"id":1247934846418027,"loadingStatus":"loaded"}},"id":1,"cell_type":"code"}
  // I am a comment
  echo "end of cell 1";
  //@bento-cell-end
  //@bento-cell:{"cell_bento_metadata":{},"id":2,"cell_type":"code"}
  echo "hi from cell 2";
  $x = 3;
  echo $x + 1;
  echo "end of cell 2";
  //@bento-cell-end
  //@bento-cell:{"cell_bento_metadata":{},"id":4,"cell_type":"code"}
  echo "this is cell 4";
  await (async () ==> {})();
  //@bento-cell-end
}
