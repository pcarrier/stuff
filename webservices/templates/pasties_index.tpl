<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"
	"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">

<html lang="en">
	<head>
		<title>Pasties</title>
	</head>
	<body>
		<form action="/" method="post" accept-charset="utf-8">
			<p>
				<label for="pastie">Pastie:</label><br/>
				<textarea name="pastie" rows="8" cols="40"></textarea>
			</p>
			<p><input type="submit" name="submit" value="Paste"></p>
		</form>
		<p>You can also send stuff through the standard input of:</p>
		<pre>$ curl -F 'pastie=<-' {{uri}}</pre>
	</body>
</html>