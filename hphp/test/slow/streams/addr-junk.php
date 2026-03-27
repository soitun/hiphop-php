<?hh

<<__EntryPoint>>
function main_addr_junk() :mixed{
  $errno = null;
  $errstr = null;
  // Use port 0 to let the OS assign an available ephemeral port,
  // avoiding "Address already in use" flakes on busy CI workers.
  $server = stream_socket_server("tcp://127.0.0.1:0", inout $errno, inout $errstr);
  if ($server === false) {
    throw new Exception("Couldn't bind server: $errstr ($errno)");
  }
  $name = stream_socket_get_name($server, false);
  $port = explode(":", $name)[1];

  $client_addr = "tcp://127.0.0.1:" . $port . "/foo/bar/baz";
  $client = stream_socket_client($client_addr, inout $errno, inout $errstr);
  $peername = null;
  $s = stream_socket_accept($server, -1.0, inout $peername);
  var_dump($client);
}
