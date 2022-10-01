The common **libfort table** life cycle:
- create a table (with default constructor, copy constructor or move constructor);
- fill it with data (`operator <<`, `operator[]`, `write_ln` ...);
- modify basic table appearance;
- convert table to string representation (`to_string`) and print it.

All resources allocated during table lifetime will be automatically freed in destructor.

Example:
```CPP
fort::char_table table;                                                
table << fort::header                                             
    << "N" << "Driver" << "Time" << "Avg Speed" << fort::endr     
    << "1" << "Ricciardo" << "1:25.945" << "47.362" << fort::endr 
    << "2" << "Hamilton" << "1:26.373" << "35.02" << fort::endr   
    << "3" << "Verstappen" << "1:26.469" << "29.78" << fort::endr;
                                                                  
std::cout << table.to_string() << std::endl;                      
```

Output:
```
+---+------------+----------+-----------+
| N | Driver     | Time     | Avg Speed |
+---+------------+----------+-----------+
| 1 | Ricciardo  | 1:25.945 | 47.362    |
| 2 | Hamilton   | 1:26.373 | 35.02     |
| 3 | Verstappen | 1:26.469 | 29.78     |
+---+------------+----------+-----------+
```