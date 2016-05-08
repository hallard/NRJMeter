var uglify = require('uglify-js');
var concat = require('concat-files');
var zlib = require('zlib');
var fs = require('fs');
var exec = require('child_process').exec, child;


var jsmain = "js/main.js";
var jsfile = "js/nrjmeter.js";
var cssfile = "css/nrjmeter.css";
var htmfile = "index.htm";
var vfile   = "version.json";

var gzjsmain = "../data/" + jsmain  + ".gz";
var gzjs     = "../data/" + jsfile  + ".gz";
var gzcss    = "../data/" + cssfile + ".gz";
var gzhtm    = "../data/" + htmfile + ".gz";

// =================
// javascript Files
// =================
var stream = fs.createWriteStream(jsfile);
stream.once('open', function(fd) {

	console.log('Uglifying .js files');
	uglified = uglify.minify([ 
		"js/ajaxq.js", 
		"js/autofill.js", 
		"js/validator.js"
 	  //'js/bootstrap-toggle.js'
	] 	); 

  stream.write(uglified.code);
  stream.end();

	console.log('Concataining already minified .js files');
	concat([
	  'js/jquery-2.1.4.min.js',
	  'js/bootstrap.min.js',
	  'js/bootstrap-table.min.js',
	  'js/bootstrap-table-fr-FR.min.js',
	  'js/bootstrap-table-en-US.min.js',
	  'js/bootstrap-notify.min.js',
	  'js/bootstrap-slider.min.js',
	  'js/bootstrap-switch.min.js',
	  'js/bootstrap-show-password.min.js',
	  'js/Chart.min.js',
	  'js/reconnecting-websocket.min.js',
		'js/jquery.mousewheel-min.js',
		'js/jquery.terminal-min.js',
	   jsfile
	], jsfile, function() {
							var gzip = zlib.createGzip();
							var inp = fs.createReadStream(jsfile);
							var out = fs.createWriteStream(gzjs);

							console.log('Compressing '+gzjs+' file');
							inp.pipe(gzip).pipe(out);
						  console.log('finished!');
						});
});


// =================
// CSS Files
// =================
console.log('Concataining already minified .css files');
concat([
  'css/bootstrap.min.css',
  'css/bootstrap-table.min.css',
  'css/bootstrap-slider.min.css',
  'css/bootstrap-switch.min.css',
  'css/jquery.terminal-min.css',
//  'css/bootstrap-toggle.min.css',
  'css/nrjmeter.min.css'
], cssfile, function() {
						var gzip = zlib.createGzip();
						var inp = fs.createReadStream(cssfile);
						var out = fs.createWriteStream(gzcss);

						console.log('Compressing '+gzcss+' file');
						inp.pipe(gzip).pipe(out);
					  console.log('finished!');
					});



// ============
// main.js file
// ============
var gzip = zlib.createGzip();
var inp = fs.createReadStream(jsmain);
var out = fs.createWriteStream(gzjsmain);

console.log('Compressing '+gzjsmain+' file');
inp.pipe(gzip).pipe(out);
console.log('finished!');

// =================
// index.html file
// =================
gzip = zlib.createGzip();
inp = fs.createReadStream(htmfile);
out = fs.createWriteStream(gzhtm);

console.log('Compressing ' + gzhtm + ' file');
inp.pipe(gzip).pipe(out);
console.log('finished!');


// ========================
// Increment version number
// ========================
var vfile_content = fs.readFileSync(vfile);
var content = JSON.parse(vfile_content);
content.build = content.build + 1;
console.log('Incrementing build to ' + content.build );
// save here
fs.writeFileSync(vfile, JSON.stringify(content,null,2));
// Save in data
fs.writeFileSync('../data/'+vfile, JSON.stringify(content,null,2));


// 'mkspiffs.exe -s 1028096 -p 256 -b 8192 -c ../data/ test.spiffs

//child = exec('ls -al',
//  function (error, stdout, stderr) {
//    console.log('stdout: ' + stdout);
//    console.log('stderr: ' + stderr);
//    if (error !== null) {
//      console.log('exec error: ' + error);
//    }
//});
