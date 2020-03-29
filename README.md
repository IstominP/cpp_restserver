

odb -I/usr/local/include/ -d mysql --generate-query --generate-schema person.hxx

./restserver --user root --database test --password root --host lab3.eltex.loc