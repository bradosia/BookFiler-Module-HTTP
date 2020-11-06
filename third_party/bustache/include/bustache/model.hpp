/*//////////////////////////////////////////////////////////////////////////////
    Copyright (c) 2014-2020 Jamboree

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//////////////////////////////////////////////////////////////////////////////*/
#ifndef BUSTACHE_MODEL_HPP_INCLUDED
#define BUSTACHE_MODEL_HPP_INCLUDED

#include <bustache/format.hpp>
#include <vector>
#include <cstring>
#include <concepts>
#include <functional>
#include <fmt/format.h>

namespace std
{
    template<class... T>
    class variant;
};

namespace bustache::detail
{
    struct vtable_base;

    template<class T>
    inline T const& deref_data(void const* p)
    {
        return *static_cast<T const*>(p);
    }

    template<class T>
    struct range_value;

    template<class T, std::size_t N>
    struct range_value<T[N]>
    {
        using type = T;
    };

    template<class T> requires requires { typename T::value_type; }
    struct range_value<T>
    {
        using type = typename T::value_type;
    };

    template<class T>
    using range_value_t = typename range_value<T>::type;

    namespace barrier
    {
        using std::begin;
        using std::end;

        struct begin_fn
        {
            template<class T>
            auto operator()(T const& seq) const -> decltype(begin(seq))
            {
                return begin(seq);
            }
        };

        struct end_fn
        {
            template<class T>
            auto operator()(T const& seq) const -> decltype(end(seq))
            {
                return end(seq);
            }
        };
    }

    constexpr barrier::begin_fn begin{};
    constexpr barrier::end_fn end{};

    template<class>
    struct fn_base;

    template<class R, class... T>
    struct fn_base<R(T...)>
    {
        fn_base() noexcept : _data(), _call() {}

        template<class F>
        fn_base(F const& f) noexcept : _data(&f), _call(call<F>) {}

        template<class F>
        fn_base(F* f) noexcept : _data(f), _call(call_fp<F>) {}

        R operator()(T... t) const
        {
            return _call(_data, std::forward<T>(t)...);
        }

        template<class F>
        static R call(void const* f, T&&... t)
        {
            return deref_data<F>(f)(std::forward<T>(t)...);
        }

        template<class F>
        static R call_fp(void const* f, T&&... t)
        {
            return static_cast<F*>(f)(std::forward<T>(t)...);
        }

        void const* _data;
        R(*_call)(void const*, T&&...);
    };

    struct content_visitor;
    struct object_ptr;
}

namespace bustache
{
    enum class model
    {
        null,
        atom,
        object,
        list,
        lazy_value,
        lazy_format
    };

    template<class T>
    struct impl_test;

    template<class T>
    struct impl_print;

    template<class T>
    struct impl_object;

    template<class T>
    struct impl_list;

    template<class T>
    struct impl_model;

    template<class T>
    struct impl_compatible;

    struct value_ptr;

    template<class T>
    concept Model = requires
    {
        {impl_model<T>::kind} -> std::convertible_to<model>;
    };

    template<class T>
    concept Compatible = requires(T const* p)
    {
        {impl_compatible<T>::get_value_ptr(*p)} -> std::same_as<value_ptr>;
    };

    template<class F>
    concept Lazy_format = requires(F const& f, ast::view const* view)
    {
        {f(view)} -> std::convertible_to<format>;
    };

    template<class T>
    concept Value = requires(T const& val)
    {
        value_ptr(&val);
    };

    template<class F>
    concept Lazy_value = requires(F const& f, ast::view const* view)
    {
        {f(view)} -> Value;
    };

    template<class T>
    concept Arithmetic = std::is_arithmetic_v<T>;

    template<class T>
    concept String = std::convertible_to<T, std::string_view> && !std::same_as<T, std::nullptr_t>;

    template<class T>
    concept ValueRange = requires(T const& t)
    {
        detail::begin(t);
        detail::end(t);
    } && Value<detail::range_value_t<T>>;

    template<class T>
    concept StrValueMap = requires(T const& t, std::string const& key)
    {
        t.find(key) == t.end();
    } && Value<typename T::mapped_type>;

    template<class T>
    concept Formattable = requires
    {
        fmt::formatter<T>{};
    };

    template<class F, class R, class... T>
    concept Callable = requires(F const& f, T... t)
    {
        {f(t...)} -> std::convertible_to<R>;
    };

    template<class>
    class fn_ref;

    template<class R, class... T>
    class fn_ref<R(T...)> : detail::fn_base<R(T...)>
    {
        using base_t = detail::fn_base<R(T...)>;

    public:
        template<Callable<R, T...> F>
        fn_ref(F const& f) noexcept : base_t(f) {}

        using base_t::operator();
    };

    template<class>
    class fn_ptr;

    template<class R, class... T>
    class fn_ptr<R(T...)> : detail::fn_base<R(T...)>
    {
        using base_t = detail::fn_base<R(T...)>;

    public:
        fn_ptr() = default;
        fn_ptr(std::nullptr_t) noexcept {}

        template<Callable<R, T...> F>
        fn_ptr(F const& f) noexcept : base_t(f) {}

        using base_t::operator();

        explicit operator bool() const { return !!this->_data; }
    };

    using output_handler = fn_ref<void(char const*, std::size_t)>;

    struct value_ptr
    {
        constexpr value_ptr() { reset(); }
        constexpr value_ptr(std::nullptr_t) { reset(); }

        template<Model T>
        value_ptr(T const* p) noexcept { p ? init_model(p) : reset(); }

        template<Compatible T>
        value_ptr(T const* p) noexcept { p ? init_compatible(p) : reset(); }

        template<Lazy_value F>
        value_ptr(F const* f) noexcept { f ? init_lazy_value(f) : reset(); }

        template<Lazy_format F>
        value_ptr(F const* f) noexcept { f ? init_lazy_format(f) : reset(); }

        constexpr explicit operator bool() const { return !!data; }

        constexpr void reset();

    private:
        template<class T>
        void init_model(T const* p) noexcept;

        template<class F>
        void init_lazy_format(F const* f) noexcept;

        template<class F>
        void init_lazy_value(F const* f) noexcept;

        template<class T>
        void init_compatible(T const* p) noexcept
        {
            *this = impl_compatible<T>::get_value_ptr(*p);
        }

        friend struct detail::content_visitor;
        friend struct detail::object_ptr;

        void const* data;
        detail::vtable_base const* vptr;
    };

    using value_handler = fn_ref<void(value_ptr)>;
}

namespace bustache::detail
{
    template<class T>
    struct type {};

    struct vtable_base
    {
        model kind;
    };

    struct lazy_format_vtable : vtable_base
    {
        template<class F>
        constexpr lazy_format_vtable(type<F>) : vtable_base{model::lazy_format}, call(call_impl<F>) {}

        format(*call)(void const*, ast::view const*);

        template<class F>
        static format call_impl(void const* self, ast::view const* view)
        {
            return deref_data<F>(self)(view);
        }
    };

    template<class F>
    constexpr lazy_format_vtable lazy_format_vt{type<F>{}};

    struct lazy_value_vtable : vtable_base
    {
        template<class F>
        constexpr lazy_value_vtable(type<F>) : vtable_base{model::lazy_value}, call(call_impl<F>) {}

        void(*call)(void const*, ast::view const*, value_handler visit);

        template<class F>
        static void call_impl(void const* self, ast::view const* view, value_handler visit)
        {
            auto const& val = deref_data<F>(self)(view);
            visit(&val);
        }
    };

    template<class F>
    constexpr lazy_value_vtable lazy_value_vt{type<F>{}};

    struct test_trait
    {
        constexpr test_trait(...) : test(test_default<true>) {}

        constexpr test_trait(type<void>) : test(test_default<false>) {}

        template<class T> requires requires{impl_test<T>{};}
        constexpr test_trait(type<T>) : test(test_impl<T>) {}

        bool(*test)(void const* self);

        template<bool Value>
        static bool test_default(void const*)
        {
            return Value;
        }

        template<class T>
        static bool test_impl(void const* self)
        {
            return impl_test<T>::test(deref_data<T>(self));
        }
    };

    struct print_trait
    {
        constexpr print_trait(...) : print(print_default) {}

        template<class T> requires requires{impl_print<T>{};}
        constexpr print_trait(type<T>) : print(print_impl<T>) {}

        void(*print)(void const* self, output_handler os, char const* spec);

        static void print_default(void const*, output_handler, char const*) {}

        template<class T>
        static void print_impl(void const* self, output_handler os, char const* spec)
        {
            return impl_print<T>::print(deref_data<T>(self), os, spec);
        }
    };

    struct object_trait
    {
        constexpr object_trait(...) : get(get_default) {}

        template<class T> requires requires{impl_object<T>{};}
        constexpr object_trait(type<T>) : get(get_impl<T>) {}

        void(*get)(void const* self, std::string const& key, value_handler visit);

        static void get_default(void const*, std::string const&, value_handler visit)
        {
            visit(nullptr);
        }

        template<class T>
        static void get_impl(void const* self, std::string const& key, value_handler visit)
        {
            return impl_object<T>::get(deref_data<T>(self), key, visit);
        }
    };

    struct list_trait
    {
        constexpr list_trait(...) : iterate() {}

        template<class T> requires requires{impl_list<T>{};}
        constexpr list_trait(type<T>) : iterate(iterate_impl<T>) {}

        void(*iterate)(void const* self, value_handler visit);

        template<class T>
        static void iterate_impl(void const* self, value_handler visit)
        {
            return impl_list<T>::iterate(deref_data<T>(self), visit);
        }
    };

    struct value_vtable : vtable_base, test_trait, print_trait, object_trait, list_trait
    {
        constexpr value_vtable(type<void> t)
            : vtable_base{model::null}
            , test_trait(t), print_trait(t), object_trait(t), list_trait(t)
        {}

        template<class T>
        constexpr value_vtable(type<T> t)
            : vtable_base{impl_model<T>::kind}
            , test_trait(t), print_trait(t), object_trait(t), list_trait(t)
        {}
    };

    template<class T>
    constexpr value_vtable value_vt{type<T>{}};

    template<model K, class T>
    struct check_model;

    template<class T>
    struct check_model<model::atom, T> : impl_test<T>, impl_print<T> {};

    template<class T>
    struct check_model<model::object, T> : impl_object<T> {};

    template<class T>
    struct check_model<model::list, T> : impl_list<T> {};
}

namespace bustache
{
    constexpr void value_ptr::reset()
    {
        data = nullptr;
        vptr = &detail::value_vt<void>;
    }

    template<class T>
    inline void value_ptr::init_model(T const* p) noexcept
    {
        static_assert(sizeof(detail::check_model<impl_model<T>::kind, T>));
        data = p;
        vptr = &detail::value_vt<T>;
    }

    template<class F>
    inline void value_ptr::init_lazy_format(F const* f) noexcept
    {
        data = f;
        vptr = &detail::lazy_format_vt<F>;
    }

    template<class F>
    inline void value_ptr::init_lazy_value(F const* f) noexcept
    {
        data = f;
        vptr = &detail::lazy_value_vt<F>;
    }

    template<>
    struct impl_model<bool>
    {
        static constexpr model kind = model::atom;
    };

    template<>
    struct impl_test<bool>
    {
        static bool test(bool self)
        {
            return self;
        }
    };

    template<>
    struct impl_print<bool>
    {
        static BUSTACHE_API void print(bool self, output_handler os, char const* spec);
    };

    template<Arithmetic T>
    struct impl_model<T>
    {
        static constexpr model kind = model::atom;
    };

    template<Arithmetic T>
    struct impl_test<T>
    {
        static bool test(T self)
        {
            return !!self;
        }
    };

    template<String T>
    struct impl_model<T>
    {
        static constexpr model kind = model::atom;
    };
    
    template<String T>
    struct impl_test<T>
    {
        static bool test(std::string_view self)
        {
            return !self.empty();
        }
    };

    template<>
    struct impl_print<std::string_view>
    {
        static BUSTACHE_API void print(std::string_view self, output_handler os, char const* spec);
    };

    template<String T>
    struct impl_print<T> : impl_print<std::string_view> {};

    template<Formattable T> requires (!String<T>)
    struct impl_print<T>
    {
        static void print(T const& self, output_handler os, char const* spec)
        {
            fmt::formatter<T> fmt;
            if (spec)
            {
                fmt::format_parse_context ctx{spec};
                fmt.parse(ctx);
            }
            fmt::memory_buffer buf;
            fmt::format_context ctx{fmt::format_context::iterator(buf), {}};
            fmt.format(self, ctx);
            os(buf.data(), buf.size());
        }
    };

    template<StrValueMap T>
    struct impl_model<T>
    {
        static constexpr model kind = model::object;
    };

    template<class T> requires (impl_model<T>::kind == model::object)
    struct impl_test<T> { impl_test() = delete; }; // Intentionally deleted.

    template<StrValueMap T>
    struct impl_object<T>
    {
        static void get(T const& self, std::string const& key, value_handler visit)
        {
            auto const found = self.find(key);
            visit(found == self.end() ? nullptr : &found->second);
        }
    };

    template<ValueRange T> requires (!String<T> && !StrValueMap<T>)
    struct impl_model<T>
    {
        static constexpr model kind = model::list;
    };

    template<class T> requires (impl_model<T>::kind == model::list)
    struct impl_test<T>
    {
        static bool test(T const& self)
        {
            return !impl_list<T>::empty(self);
        }
    };

    template<ValueRange T>
    struct impl_list<T>
    {
        static bool empty(T const& self)
        {
            return std::empty(self);
        }

        static void iterate(T const& self, value_handler visit)
        {
            for (auto const& elem : self)
                visit(&elem);
        }
    };

    template<String K, Value V>
    struct impl_model<std::pair<K, V>>
    {
        static constexpr model kind = model::object;
    };

    template<String K, Value V>
    struct impl_object<std::pair<K, V>>
    {
        static void get(std::pair<K, V> const& self, std::string const& key, value_handler visit)
        {
            if (key == "key")
                return visit(&self.first);
            if (key == "value")
                return visit(&self.second);
            return visit(nullptr);
        }
    };

    template<>
    struct impl_compatible<std::nullptr_t>
    {
        static value_ptr get_value_ptr(std::nullptr_t)
        {
            return nullptr;
        }
    };

    template<Value... T>
    struct impl_compatible<std::variant<T...>>
    {
        static value_ptr get_value_ptr(std::variant<T...> const& self)
        {
            return visit([](auto const& val) { return value_ptr(&val); }, self);
        }
    };
}

#endif