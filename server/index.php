<?php if (!isset($_GET['log'])) { ?>

<html>
  <head>
  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js"></script>
  <script>

	var interval = 3000;  // 1000 = 1 second, 3000 = 3 seconds

	function doAjax() {
	    $.ajax({
		    type: 'POST',
		    url: '/?log=1',
		    success: function (data) {
			    $('#log').html(data);     
		    },
		    complete: function (data) {
			    setTimeout(doAjax, interval);
		    }
	    });
	}

	//setTimeout(doAjax, interval);

	</script>
   </head>
<body>
<img width="640" height="480" src="http://192.168.0.3:8080/?action=stream" />

<ul id="log">
 
</ul>

</body>
<?php } ?>

<?php

$dir = '/home/pi/scripts';

if (isset($_GET['log'])) {

   $fd=dio_open('/dev/ttyS0',O_RDWR | O_NOCTTY | O_NDELAY);
   dio_fcntl($fd,F_SETFL,0);

  dio_write($fd,"\x41",1);  // write 0x41 or 'A' to /dev/ttyS0
        
 // Replace result_length with your expected command result length
  for ($i=0;$i < result_length;$i++) {
              $result .=dio_read($fd, 1);
        }
       echo $result;


//   echo shell_exec('tail -n 1 /dev/ttyUSB0');
   exit;
}

$command = isset($_GET['cmd']) ? $_GET['cmd'] : '';

if (!$command) {
   exit(1);
}
?>

