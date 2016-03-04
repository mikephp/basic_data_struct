#!/bin/python
#encoding=utf-8
import os,time,sys
word_lst = []
word_dict = {}

#encoding=utf-8
def word_statistics(input_file,output_file):
	path = sys.path[0]
	debug = 1
	if debug:
		print path
	with open(path+"\\"+input_file,"r") as f1 ,open(path+"\\"+output_file,'w') as f2:
		for line in f1:
			tmp = line.split('/')
			#print type(tmp)
			#print tmp
			word_lst.append(tmp)
		#print word_lst
		for item in word_lst:
			#print item
			for item2 in item:
				if item2.strip() not in "，！。“”" :
					if   item2 not in word_dict:
						word_dict[item2] = 1
					else :
						word_dict[item2] += 1
		
		for key in word_dict:
			#print key,word_dict[key]
			f2.write(key+' '+str(word_dict[key])+'\n')

word_statistics("zw.txt","css.txt")