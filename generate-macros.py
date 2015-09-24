#!/usr/bin/python3


import os


def try_prepend_typespec(name):
	if name:
		return 'system_variant_t::' + name
	else:
		return ''

def output_begin_boilerplate(to):
	for custominclude in ['sysprops']:
		to.write('#include "../' + custominclude + '.hpp"\n')
	for stdinclude in ['utility', 'vector', 'map']:
		to.write('#include <' + stdinclude + '>\n')
	to.write('\n'
	         '\n'
	         'using namespace std;\n'
	         '\n'
	         'using macros_t          = map<string, string>;\n'
	         'using system_variants_t = vector<system_variant_t>;\n'
	         '\n'
	         'using macro_defaults_t = vector<pair<const system_variants_t, const macros_t>>;\n'
	         '\n'
	         '\n');
	for (prefix, suffix) in [('extern ', ';\n'), ('', '({')]:
		to.write(prefix + 'const macro_defaults_t macro_defaults' + suffix + '\n')

def output_end_boilerplate(to):
	to.write('});\n')

def tokenize_and_output(line, name, islast, to):
	line = line[:-1]
	if line != '':
		(before, _, after) = line.partition('=')
		to.write('\t{{' + ', '.join(map(try_prepend_typespec, before.split(','))) + '}, {{"' + name + '", "' + after + '"}}}' + ('' if islast else ',') + '\n')
	else:
		to.write('\n')


try:
	os.mkdir('src/gen')
except:
	pass

with open('src/gen/macro_defaults.cpp', 'w') as output:
	output_begin_boilerplate(output)
	alllines = []
	for filename in os.listdir('macros'):
		print('Found macro package "' + filename + '"')
		with open('macros/' + filename, 'r') as file:
			alllines += file.readlines() + ['\n']
	for line in alllines[:-2]:
		tokenize_and_output(line, filename, False, output)
	tokenize_and_output(alllines[-2], filename, True, output)
	output_end_boilerplate(output)
