/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BUSTACHE_AST_HPP_INCLUDED
#define BUSTACHE_AST_HPP_INCLUDED

#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>

namespace bustache::ast
{
    enum class type
    {
        null,
        text,
        var_escaped,
        var_raw,
        section,
        inversion,
        filter,
        loop,
        inheritance,
        partial
    };

    struct content
    {
        type kind = type::null;
        unsigned index;

        bool is_null() const { return kind == type::null; }
    };

    using text = std::string_view;

    using content_list = std::vector<content>;

    using override_map = std::unordered_map<std::string, content_list>;

    struct variable
    {
        std::string key;
        unsigned split;
    };

    struct block
    {
        std::string key;
        content_list contents;
    };

    struct partial
    {
        std::string key;
        std::string indent;
        override_map overriders;
    };

    struct context
    {
        std::vector<text> texts;
        std::vector<variable> variables;
        std::vector<block> blocks;
        std::vector<partial> partials;

        content add(text node)
        {
            content ret{type::text, unsigned(texts.size())};
            texts.push_back(node);
            return ret;
        }

        content add(type kind, variable&& node)
        {
            content ret{kind, unsigned(variables.size())};
            variables.push_back(std::move(node));
            return ret;
        }

        content add(type kind, block&& node)
        {
            content ret{kind, unsigned(blocks.size())};
            blocks.push_back(std::move(node));
            return ret;
        }

        content add(partial&& node)
        {
            content ret{type::partial, unsigned(partials.size())};
            partials.push_back(std::move(node));
            return ret;
        }

    private:
        template<type K>
        struct tag;

        struct variable_tag;
        struct block_tag;

    public:
        template<type K>
        typename tag<K>::type const* get_if(content c) const
        {
            return c.kind == K ? tag<K>::data(*this) + c.index : nullptr;
        }

        template<type K>
        typename tag<K>::type* get_if(content c)
        {
            return c.kind == K ? tag<K>::data(*this) + c.index : nullptr;
        }

        template<class F>
        auto visit(F&& f, content c) const -> decltype(auto)
        {
            switch (c.kind)
            {
            case type::null: return f(c.kind, static_cast<void*>(nullptr));
            case type::text: return f(c.kind, texts.data() + c.index);
            case type::var_escaped:
            case type::var_raw:
                return f(c.kind, variables.data() + c.index);
            case type::section:
            case type::inversion:
            case type::filter:
            case type::loop:
            case type::inheritance:
                return f(c.kind, blocks.data() + c.index);
            case type::partial: return f(c.kind, partials.data() + c.index);
            }
        }
    };

    template<>
    struct context::tag<type::text>
    {
        using type = text;
        template<class Self>
        static auto data(Self& self) { return self.texts.data(); }
    };

    struct context::variable_tag
    {
        using type = variable;
        template<class Self>
        static auto data(Self& self) { return self.variables.data(); }
    };

    template<> struct context::tag<type::var_escaped> : variable_tag {};
    template<> struct context::tag<type::var_raw> : variable_tag {};

    struct context::block_tag
    {
        using type = block;
        template<class Self>
        static auto data(Self& self) { return self.blocks.data(); }
    };

    template<> struct context::tag<type::section> : block_tag {};
    template<> struct context::tag<type::inversion> : block_tag {};
    template<> struct context::tag<type::filter> : block_tag {};
    template<> struct context::tag<type::loop> : block_tag {};
    template<> struct context::tag<type::inheritance> : block_tag {};

    template<>
    struct context::tag<type::partial>
    {
        using type = partial;
        template<class Self>
        static auto data(Self& self) { return self.partials.data(); }
    };

    struct document
    {
        context ctx;
        content_list contents;
    };

    struct view
    {
        context const& ctx;
        content_list const& contents;
    };
}

#endif