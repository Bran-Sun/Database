//
// Created by 孙桢波 on 2018-12-23.
//

#include <cstdio>
#include <iostream>
#include "../parser/Parser.h"
#include "SystemManager.h"
#include <fstream>

unsigned char MyBitMap::h[61]; //defination

int main(int argc, char *argv[]) {
    MyBitMap::initConst();   //新加的初始化
    SystemManager system;
    
    using namespace parser;
    Parser par;
    std::vector<std::shared_ptr<Action>> actions;
    
    std::ifstream fin;
    std::string command;
    clock_t start, end;
    
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            command.clear();
            fin.open(argv[i]);
            if (fin.is_open()) {
                std::string tem;
                while (std::getline(fin, tem)) {
                    command = command + tem;
                }
                fin.close();
            }
            
            try {
                actions = par.parse(command);
            } catch (const Error &e) {
                printf("%s", e.what());
            }
            
            for (auto &act: actions) {
                start = clock();
                try {
                    printf("mysql_show>\n");
                    act->show();
                    act->execute(system);
                } catch (const Error &e) {
                    printf("%s", e.what());
                }
                end = clock();
                cout<<"Run time: "<<(double)(end - start) / CLOCKS_PER_SEC<<"S"<<endl;
            }
        }
    }
    
    while (true) {
        printf("mysql> ");
        getline(std::cin, command);
        if (command == "QUIT" || command == "QUIT;") {
            break;
        }
        try {
            actions = par.parse(command);
        } catch (const Error &e) {
            printf("%s", e.what());
            continue;
        }
        
        for (auto &act: actions) {
            start = clock();
            try {
                printf("mysql_show>\n");
                act->show();
                act->execute(system);
            } catch (const Error &e) {
                printf("%s", e.what());
            }
            end = clock();
            cout<<"Run time: "<<(double)(end - start) / CLOCKS_PER_SEC<<"S"<<endl;
        }
    }
    
    /*
    std::string command("CREATE DATABASE orderDB;\n"
                        "USE orderDB;\n"
                        "CREATE TABLE customer(\n"
                        "\tid INT(10) NOT NULL,\n"
                        "\tname CHAR(25) NOT NULL,\n"
                        "\tgender CHAR(1) NOT NULL,\n"
                        "\tPRIMARY KEY (id)\n"
                        ");\n"
                        "SHOW TABLES;\n"
                        "DESC customer;\n"
                        );
    
    std::string initCommand("CREATE DATABASE orderDB;\n"
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
    
    std::string showCommand("USE orderDB;\n"
                            "DESC food;\n");
    
    std::string insertCommand("USE orderDB;\n"
                              "INSERT INTO `food` VALUES (200001,1613,'Boiled dumplings',64.0236),(200002,1768,'Waffles',80.5322);"
                              );
    
    std::string selectCommand("SELECT * FROM customer WHERE id = 314000;\n");
    
    std::string updateCommand("USE orderDB;\n"
                              "UPDATE customer SET name = 'BRAN' WHERE id=300003;\n"
                              "SELECT name FROM customer WHERE gender = 'M';\n");
    
    std::string deleteCommand("USE orderDB;\n"
                              "DELETE FROM customer WHERE id = 300001;\n"
                              "SELECT name FROM customer WHERE gender = 'F';\n");
    
    std::string useCommand("USE orderDB;\n");
    
    std::string showDatabases("SHOW DATABASES;\n");
    std::string showTables("SHOW TABLES;\n");
    std::string descTable("DESC orders;\n");
    std::string selectOrders("SELECT * FROM customer WHERE date > '2016-09-12';\n");
    std::string foreignTest("INSERT INTO `restaurant` VALUES (1001,'apple pan the','10801 w. pico blvd.','310-475-3585',3.5),(1002,'asahi ramen','2027 sawtelle blvd.','310-479-2231',4.25),(1003,'baja fresh','3345 kimber dr.','805-498-4049',3.71);\n"
                            "INSERT INTO `food` VALUES (200001,1001,'Boiled dumplings',64.0236),(200002,1001,'Waffles',80.5322);\n"
                            );
    
    std::string foreignTest2("DELETE FROM restaurant WHERE id = 1002;\n");
    std::string foreignTest3("INSERT INTO `food` VALUES (200030,1001,'Boiled dumplings',64.0236),(200031,1001,'Waffles',80.5322);\n");
    std::string foreignTest4("UPDATE food SET restaurant_id = 1002 WHERE id=200001;\n"
                             "SELECT * FROM food WHERE id > 0;\n");
    std::string foreignTest5("SELECT * FROM restaurant WHERE id > 0;\n");
    std::string universe("SELECT * FROM restaurant WHERE id < 10000;\n");
    
    std::string doubleSelect("SELECT * FROM restaurant WHERE id < 10000;\n"
                             "SELECT * FROM food WHERE id > 0;\n"
            "SELECT * FROM restaurant, food WHERE food.restaurant_id = restaurant.id;\n");
    
    std::ifstream fin("customer.sql");
    std::string insertCustomer;
    getline(fin, insertCustomer);
    fin.close();
    
    fin.open("food.sql");
    std::string insertFood;
    getline(fin, insertFood);
    fin.close();
    
    fin.open("restaurant.sql");
    std::string insertResturant;
    getline(fin, insertResturant);
    fin.close();
    
    fin.open("orders.sql");
    std::string insertOrders;
    getline(fin, insertOrders);
    fin.close();
    
    using namespace parser;
    Parser par;
    
    std::vector<std::shared_ptr<Action>> actions;
    
    try {
        actions = par.parse();
    } catch (const Error &e) {
        printf("%s", e.what());
    }
    
    clock_t start, end;
    start = clock();
    
    for (auto act : actions)
    {
        try {
            act->show();
            act->execute(system);
        } catch (const Error &e) {
            printf("%s", e.what());
        }
    }
    
    end = clock();
    cout<<"Run time: "<<(double)(end - start) / CLOCKS_PER_SEC<<"S"<<endl;
     */
}
