#!/usr/bin/env ruby

18.times do |num|
	num += 1
	cmd = "./err-detect 500 dataset/boundingbox_test.txt dataset/keypoints_test.txt num-model/num_#{num*100}.model"
	#puts cmd
	res = `#{cmd} | tail -n 1`
	puts "#{num*100}\t#{res}"
end
