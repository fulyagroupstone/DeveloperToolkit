# DeveloperToolkit
My developer toolkit writed in c++ to make project magment easier. You can use it as well and sugest what I should add.

# Tasks
1. init -n [name] { -t [template_name] -T [template_file] }  -  creates new project (if -t then creates new single-solution project)
2. info  -  prints all data abound the project, validates ".project/project" file
3. enable [name]  -  validates ".project/project" and enables functionality [log|test]
4. disable [name]  -  oposite for enable

# Templates
1. c_cpp - default template for simple c/c++ project and modules (executable)
2. c - (in file "test/templates/c.txt") simpler template for c only (uses gcc)
3. cpp - (in file "test/templates/cpp.txt") simpler template for c/c++ (uses g++)

# Building 
Just run "run".
In some cases you must add "execute" permision to files in ".scripts" and to "run" file.
