#!/usr/bin/env ruby

$input_path = ARGV[0]
$output_path = ARGV[1]

def usage()
	puts "./resize.rb input_path output_path"
end

if ARGV.length != 2
	usage
	exit 0
end

img_list = Dir[$input_path+"/*"]

img_list.each do |fn|
	name = fn.split("/")[-1]
	final_name = $output_path + "/" + name
	`convert -resize 10000x100 #{fn} #{final_name}`
end
