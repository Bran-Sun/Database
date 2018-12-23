//
// Created by 孙桢波 on 2018-12-20.
//

#include <cstdio>
#include <iostream>
#include "Lexer.h"
#include "Parser.h"

int main() {
    using namespace parser;
    Parser par;
    std::vector<std::shared_ptr<Action>> actions = par.parse("CREATE DATABASE orderDB;\n"
                                                             "\n"
                                                             "USE orderDB;\n"
                                                             "\n"
                                                             "CREATE TABLE restaurant (\n"
                                                             "  id INT(10) NOT NULL,\n"
                                                             "  name CHAR(25) NOT NULL,\n"
                                                             "  address CHAR(100),\n"
                                                             "  phone CHAR(20),\n"
                                                             "  rate FLOAT,\n"
                                                             "  PRIMARY KEY (id)\n"
                                                             ");\n"
                                                             "\n"
                                                             "CREATE TABLE customer(\n"
                                                             "\tid INT(10) NOT NULL,\n"
                                                             "\tname CHAR(25) NOT NULL,\n"
                                                             "\tgender CHAR(1) NOT NULL,\n"
                                                             "\tPRIMARY KEY (id)\n"
                                                             ");\n"
                                                             "\n"
                                                             "CREATE TABLE food(\n"
                                                             "\tid INT(10) NOT NULL,\n"
                                                             "\trestaurant_id INT(10),\n"
                                                             "\tname CHAR(100) NOT NULL,\n"
                                                             "\tprice FLOAT NOT NULL,\n"
                                                             "\tPRIMARY KEY (id),\n"
                                                             "\tFOREIGN KEY (restaurant_id) REFERENCES restaurant(id)\n"
                                                             ");\n"
                                                             "\n"
                                                             "CREATE TABLE orders(\n"
                                                             "\tid INT(10) NOT NULL,\n"
                                                             "\tcustomer_id INT(10) NOT NULL,\n"
                                                             "\tfood_id INT(10) NOT NULL,\n"
                                                             "\tdate DATE,\n"
                                                             "\tquantity INT(10),\n"
                                                             "\tPRIMARY KEY (id),\n"
                                                             "\tFOREIGN KEY (customer_id) REFERENCES customer(id),\n"
                                                             "\tFOREIGN KEY (food_id) REFERENCES food(id)\n"
                                                             ");");
    
    for (auto act : actions) {
        act->show();
    }
}
