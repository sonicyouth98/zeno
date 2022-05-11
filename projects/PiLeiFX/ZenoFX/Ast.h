//
// Created by admin on 2022/5/6.
//

#pragma once

#include "Lexical.h"
#include "Location.h"
#include <iostream>
#include <memory>
#include <string>
#include <any>
#include <memory>
#include <vector>
namespace zfx {
    enum class Ast_Node_Type {
        Ast_Type_Statement,
        Ast_Type_Declaration,
        Ast_Type_Identifier,
        Ast_Type_Integer_Literal,
        Ast_Type_Binary_Op,
        Ast_Type_Unary_Op
    };
    class AstNode;
    class Variable;
    class Binary;
    class Unary;

    class AstVisitor {
        virtual ~visitor() {

        }

        virtual std::any visitVariable(Variable& variable, std::string additional = "");

        virtual std::any visitFunctionCall(FunctionCall& functionCall, std::string additional = "");

        virtual std::any visitBinary(Binary& binary, std::strig additional = "");

        virtual std::any visitUnary(Unary& unary, std::string additional = "");

        virtual std::any visitTenary(Tenary& tenary, std::string additional = "");

        virtual std::any visitAssign(AssignStmt& assign, std::string additional = "");

        virtual std::any visitLiteral(Literal& literal, std::string additional = "");

        virtual std::any visitIfStmt(ExprIfStmt& exprIfStmt, std::string additional = "");

        virtual std::any visitForStmt()
    };


    class AstNode {
      public:
        Position beginPos;//
        Position endPos;
        bool isErrorNode {false};

        virtual std::any accept(AstVisitor& visitor, std::string additional = "") = 0;
    };

    class Statement : public AstNode {
      public:
        int id;
        int dim = 0;
        Statement(const Position& beginPos, const Position& endPos, bool isErrorNode, int id, int dim)
            : AstNode(beginPos, endPos, isErrorNode),id(id), dim(dim) {

        }
    };


    class Ast_Identifier {

    };

    class Expression : public AstNode{

    };

    class Variable : public Expression {
        std::string name;
        //if we
        //std::shared_ptr<>;

        std::string toString() {
            return this->name;
        }
    };

    class FunctionCall : public Expression {
      public:
        std::string name;
        std::vector<std::shared_ptr<AstNode>> arguments;
        FunctionCall() {

        }

        virtual std::string
    };

    class Binary {
      public:
        Op op;
        std::shared_ptr<AstNode> exp1;// left expression
        std::shared_ptr<AstNode> exp2;// right expression
        Binary() {

        }
        std::string toString() {

        }
    };

    class Unary {
      public:
        Op op;
        bool isPrefix;//whether is prefix operation;
        Unary() {

        }
    };

    class Tenary {
        //Tenary Operation;
      public:
        std::shared_ptr<AstNode> cond;
        std::shared_ptr<AstNode> lhs;
        std::shared_ptr<AstNode> rhs;
    };

    class AssignStmt {
      public:
        std::shared_ptr<Expression> lhs;
        std::shared_ptr<Expression> value_to_assign;
    };


    class ExprIfStmt {

    };


    class Literal {

    };
}

