//
// Created by 孙桢波 on 2018-12-23.
//

#include <cstdio>
#include <iostream>
#include "../parser/Parser.h"
#include "SystemManager.h"

unsigned char MyBitMap::h[61]; //defination

int main() {
    MyBitMap::initConst();   //新加的初始化
    SystemManager system;
    std::string command("CREATE DATABASE orderDB;\n"
                        "USE orderDB;\n"
                        "CREATE TABLE customer(\n"
                        "\tid INT(10) NOT NULL,\n"
                        "\tname CHAR(25) NOT NULL,\n"
                        "\tgender CHAR(1) NOT NULL,\n"
                        "\tPRIMARY KEY (id)\n"
                        ");\n"
                        "SHOW TABLES;\n"
                        "INSERT INTO `customer` VALUES (300001,'CHAD CABELLO','F');\n"
                        );
    
    std::string showCommand("USE orderDB;\n"
                            "DESC customer;\n");
    
    using namespace parser;
    Parser par;
    std::vector<std::shared_ptr<Action>> actions = par.parse(showCommand);
    
    for (auto act : actions)
    {
        act->show();
        act->execute(system);
    }
}
