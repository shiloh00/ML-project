#!/usr/bin/env ruby

18.times do |num|
	num += 1
	cmd = "./esr-train-num #{num*100} dataset/boundingbox.txt dataset/keypoints.txt num-model/num_#{num*100}.model"
	puts cmd
	`#{cmd}`
end
