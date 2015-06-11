#!/usr/bin/env ruby

$input_path = ARGV[0]
$output_path = ARGV[1]

def usage()
	puts "./re_order.rb input_path output_path"
end

if ARGV.length != 2
	usage
	exit 0
end

img_list = Dir[$input_path+"/*"]
$cur_idx = 0
img_list.each do |fn|
	final_name = $output_path + "/" + "%02d.jpg" % $cur_idx
	`cp #{fn} #{final_name}`
	$cur_idx += 1
end
