#! /usr/bin/env python3

import os

# 检查文件类型是否正确
def file_type_is_right(file_name):
        ret = False
        file_suffix = list(file_name.split(".")).pop()
        if file_suffix == 'c' or file_suffix == 'cc' or file_suffix == 'cpp' \
		  or file_suffix == 'cxx':
                ret = True
        return ret

#strip unneccsary info of file path name
def file_path_strip_name(name):
        file_path_name = name
        if os.sep != '/':
                file_path_name = file_path_name.replace(os.sep, '/')
        while file_path_name[0] == '/':
                file_path_name = file_path_name[1:]
        while file_path_name[-1] == '/':
                file_path_name = file_path_name[:-2]
        return file_path_name

# 取得对应文件的文件名
def get_file_name(file_dir):
        L = []
        for root, dirs, files in os.walk(file_dir):
                for file in files:
                        if file_type_is_right(file):
                                file_path_name = list(os.path.join(root, file).split(file_dir)).pop()
                                L.append(file_path_strip_name(file_path_name))
        return L

if __name__ == '__main__':
        root_path     = "../"
        src_path      = ("Src", " ")
        object_mkfile = "./scripts/objects.mk"
        compile_cpt = "\t$(call compile_c)"
        object_name = []

        for file_path in src_path: #
                for x in get_file_name(root_path + file_path):
                        file_name = (file_path.replace(root_path, '') + '/' \
                                + x).replace('/', '_')
                        object_name.append(list(file_name.split(".c"))[0]+ ".o")

        f = open(object_mkfile, "w", encoding="UTF-8")
        f.write("#user's object files:\n")
        f.write("USEROBJS = ")
        for x in object_name:
                f.write(" \\\n    ")
                f.write("$(OBJDIR)/" + x)

        f.write("\n#user's object compile:\n")
        for file_path in src_path:
                for x in get_file_name(root_path + file_path):
                        file_name = (file_path.replace(root_path, '') + '/' \
                                + x).replace('/', '_')
                        file_suffix = file_name.split('.').pop()
                        object_n = list(file_name.split("." + file_suffix))[0]+".o"
                        object_n = "$(OBJDIR)/" + object_n
                        object_path = "$(ROOTDIR)"+ '/' \
                                + file_path_strip_name(file_path) + '/'
                        f.write(object_n)
                        f.write(": ")
                        f.write(object_path)
                        f.write(x)
                        f.write("\n")
                        compile_cmd = compile_cpt
                        if file_suffix == 'cxx' or file_suffix == 'cpp' or file_suffix == 'cc':
                                compile_cmd = compile_cmd.replace("CC ", "CXX")
                                compile_cmd = compile_cmd.replace("$(CC)", "$(CXX)")
                                compile_cmd = compile_cmd.replace("$(CFLAGS)", "$(CXXFLAGS)")
                        f.write(compile_cmd)
                        f.write("\n")
        f.close()
