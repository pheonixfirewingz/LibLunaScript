#pragma once
#include "../AST.h"
#include <map>
#include <string>

namespace LunaScript::compiler::back
{
using namespace LunaScript::compiler::ast;
class CodeGenerator
{
    std::map<uint64_t, std::string> global_map;
    class RegisterTracker
    {
        struct Register
        {
          private:
            bool is_free = true;
            std::string whats_in_it = "";

          public:
            void set(std::string data)
            {
                is_free = false;
                whats_in_it = data;
            }

            std::string get() const noexcept
            {
                return whats_in_it;
            }

            bool isFree() const noexcept
            {
                return is_free;
            }

            void clear() noexcept
            {
                is_free = true;
                whats_in_it = "";
            }
        } registers[7];
        uint8_t last_used_reg = 0;

      public:
        bool setFreeReg(std::string data)
        {
            for (uint8_t i = 0; i < 7; i++)
            {
                if (registers[i].isFree())
                {
                    registers[i].set(data);
                    last_used_reg = i;
                    return true;
                }
            }
            return false;
        }

        void clearReg(std::string data)
        {
            for (uint8_t i = 0; i < 7; i++)
            {
                if (registers[i].get() == data)
                {
                    registers[i].clear();
                    return;
                }
            }
        }

        uint8_t getLastUsedReg() const noexcept
        {
            return last_used_reg + 1;
        }

        std::string getReg(std::string data) noexcept
        {
            for (uint8_t i = 0; i < 7; i++)
                if (registers[i].get() == data)
                {
                    last_used_reg = i;
                    return std::string("r") += std::to_string(i + 1);
                }
        }

        std::string getReg(const uint8_t i) noexcept
        {
            return std::string("r") += std::to_string(i);
        }

        void wipe()
        {
            for (uint8_t i = 0; i < 7; i++)
                    registers[i].clear();
        }
    } tracker;
    const ASTRoot *root;
    std::string ret;
    void error();

    struct BinaryInfo
    {
        const uint16_t precedence;
        const OperatorType op;
        const ASTLiteral *left;
        const ASTLiteral *right;
        BinaryInfo(const uint16_t precedence_in, const OperatorType op_in, const ASTLiteral *left_in,
                   const ASTLiteral *right_in)
            : precedence(precedence_in)
            , op(op_in)
            , left(left_in)
            , right(right_in)
        {
        }
    };
    void genFunction(const ASTFuncDef *);
    void genExpression(const ASTExpression *);

  public:
    explicit CodeGenerator(const ASTRoot *root)
        : root(root)
    {
    }

    std::string generate();
};
} // namespace LunaScript::compiler::back