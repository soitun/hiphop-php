<?hh

<<__EntryPoint>>
function main_tcp_transport_is_assumed() :mixed{
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

  $peername = null;
  $stream = stream_socket_client('127.0.0.1:'.$port, inout $errno, inout $errstr);
  stream_socket_accept($server, -1.0, inout $peername);
  var_dump(stream_get_meta_data($stream)['wrapper_type']);
  $stream = stream_socket_client('tcp://127.0.0.1:'.$port, inout $errno, inout $errstr);
  stream_socket_accept($server, -1.0, inout $peername);
  var_dump(stream_get_meta_data($stream)['wrapper_type']);
  $stream = stream_socket_client('udp://127.0.0.1:'.$port, inout $errno, inout $errstr);
  var_dump(stream_get_meta_data($stream)['wrapper_type']);
}
