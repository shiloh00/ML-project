var express = require('express');
var router = express.Router();
var multer = require("multer");
var uuid = require("uuid");
var net = require("net");


var img_dir = "/home/yangy/course/ML-project/esr-webdemo/public/images/";


/*
if (typeof String.prototype.startsWith != 'function') {
	// see below for better implementation!
	String.prototype.startsWith = function (str){
		return this.indexOf(str) === 0;
	};
}
*/

router.use(multer({
	dest: img_dir,
	rename: function(fieldname, filename, req, res) {
		var new_name = uuid.v4();
		console.log("rename => "+new_name);
		return new_name;
	},
	onFileUploadComplete: function(file, req, res) {
		console.log("uploaded: "+file.name);
		res.end(JSON.stringify({uuid: file.name}));
	},
	onError: function(error, next) {
		console.log(error);
		next(error);
	}
}));

router.post('/upload', function(req, res, next) {
	console.log("hehe");
});

router.post('/do_alignment', function(req, res, next) {
	var data = "";
	var uuid = req.body.uuid;
	console.log("hehe => " + JSON.stringify(req.body));
	if(uuid == undefined) {
		res.end(JSON.stringify({"error": "no uuid is provided for alignment"}));
		console.log("no uuid is provided");
		return;
	}
	console.log("uuid parsed == "+uuid);
	var sock = net.connect("/tmp/alignment.sock");
	console.log("come");
	res.set({
		'Content-Type': 'application/json',
	});
	sock.on('connect', function() {
		// TODO: extract the UUID from the request body
		var path = "/home/yangy/course/ML-project/esr-webdemo/public/img/downey.jpg";
		console.log("pre-path => "+uuid);
		if(uuid.indexOf("/") != 0) {
			path = img_dir + uuid;
		}
		console.log("path => "+path);
		console.log("connected");
		sock.write(path);

	});
	sock.on('data', function(chunk) {
		console.log("data!!");
		data += chunk;
	});
	sock.on('end', function() {
		console.log("end!!"+data);
		var faceList = data.split(" ");
		var faces = [];
		if(faceList[0].length > 2) {
			for(var i = 0; i < faceList.length; i++) {
				var pts = faceList[i].split(",");
				var curFace = [];
				for(var j = 0; j < pts.length; j += 2) {
					curFace.push({
						x: parseFloat(pts[j]),
						y: parseFloat(pts[j+1]),
					});
				}
				faces.push(curFace);
			}
		}
		console.log(faces);
		res.end(JSON.stringify({"uuid":uuid, "faces": faces}));
	});
	sock.on('error', function(err) {
		res.end(JSON.stringify({"error": "something error when using socket"}));
	});
});

module.exports = router;
