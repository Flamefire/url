//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

// Test that header file is self-contained.
#include <boost/url/params_encoded_ref.hpp>

#include <boost/url/url.hpp>
#include <boost/url/url_view.hpp>
#include <boost/static_assert.hpp>
#include <boost/core/ignore_unused.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

BOOST_STATIC_ASSERT(
    ! std::is_default_constructible<
        params_encoded_ref>::value);

BOOST_STATIC_ASSERT(
    std::is_copy_constructible<
        params_encoded_ref>::value);

BOOST_STATIC_ASSERT(
    std::is_copy_assignable<
        params_encoded_ref>::value);

/*  Legend

    '#' 0x23
    '%' 0x25
    '&' 0x26
    '=' 0x3d
*/
struct params_encoded_ref_test
{
    static
    bool
    is_equal(
        param_pct_view const& p0,
        param_pct_view const& p1)
    {
        return
            p0.key == p1.key &&
            p0.has_value == p1.has_value &&
            (! p0.has_value ||
                p0.value == p1.value);
    }

    static
    void
    check(
        params_encoded_ref const& p,
        std::initializer_list<
            param_pct_view> init)
    {
        if(! BOOST_TEST_EQ(
            p.size(), init.size()))
            return;
        auto it0 = p.begin();
        auto it1 = init.begin();
        auto const end = init.end();
        while(it1 != end)
        {
            BOOST_TEST(is_equal(
                *it0, *it1));
            auto tmp = it0++;
            BOOST_TEST_EQ(++tmp, it0);
            ++it1;
        }
        // reverse
        if(init.size() > 0)
        {
            it0 = p.end();
            it1 = init.end();
            do
            {
                auto tmp = it0--;
                BOOST_TEST_EQ(--tmp, it0);
                --it1;
                BOOST_TEST(is_equal(
                    *it0, *it1));
            }
            while(it1 != init.begin());
        }
    }

    static
    void
    modify(
        string_view s0,
        string_view s1,
        void(*f)(params_encoded_ref&))
     {
        url u("http://user:pass@www.example.com/path/to/file.txt?k=v#f");
        if(! s0.data())
        {
            u.remove_query();
            BOOST_TEST_EQ(u.encoded_query(), "");
        }
        else
        {
            u.set_encoded_query(s0);
            BOOST_TEST_EQ(u.encoded_query(), s0);
        }
        auto p = u.encoded_params();
        f(p);
        if(! s1.data())
        {
            BOOST_TEST(! u.has_query());
            BOOST_TEST_EQ(u.encoded_query(), "");
            BOOST_TEST(u.query() == "");
        }
        else
        {
            BOOST_TEST(u.has_query());
            BOOST_TEST_EQ(u.encoded_query(), s1);
        }
    }

    //--------------------------------------------

    static
    void
    assign(
        params_encoded_ref& p,
        std::initializer_list<param_pct_view> init)
    {
        p.assign(
            init.begin(), init.end());
    };

    static
    auto
    append(
        params_encoded_ref& p,
        std::initializer_list<param_pct_view> init) ->
            params_encoded_ref::iterator
    {
        return p.append(
            init.begin(), init.end());
    };

    static
    auto
    insert(
        params_encoded_ref& p,
        params_encoded_ref::iterator before,
        std::initializer_list<param_pct_view> init) ->
            params_encoded_ref::iterator
    {
        return p.insert(before,
            init.begin(), init.end());
    };

    static
    auto
    replace(
        params_encoded_ref& p,
        params_encoded_ref::iterator from,
        params_encoded_ref::iterator to,
        std::initializer_list<param_pct_view> init) ->
            params_encoded_ref::iterator
    {
        return p.replace(from, to,
            init.begin(), init.end());
    };

    //--------------------------------------------

    void
    testSpecial()
    {
        // params_encoded_ref(params_encoded_ref)
        {
            url u;
            params_encoded_ref p0 = u.encoded_params();
            BOOST_TEST_EQ(&p0.url(), &u);
            params_encoded_ref p1(p0);
            BOOST_TEST_EQ(
                &p0.url(), &p1.url());
            check(p0, {});
            check(p1, {});
        }

        // operator=(params_encoded_ref)
        {
            url u0( "?key=value" );
            url u1;
            params_encoded_ref p0 = u0.encoded_params();
            params_encoded_ref p1 = u1.encoded_params();
            p1 = p0;
            BOOST_TEST_NE(&p0.url(), &p1.url());
            check(p0, {{ "key", "value" }});
            check(p1, {{ "key", "value" }});
        }
    }

    void
    testModifiers()
    {
        using std::next;

        //----------------------------------------

        //
        // operator=(initializer_list)
        //

        modify(
            {},
            {},
            [](params_encoded_ref& p)
            {
                p = {};
            });

        modify(
            {},
            "y",
            [](params_encoded_ref& p)
            {
                p = {{ "y" }};
            });

        modify(
            {},
            "y=",
            [](params_encoded_ref& p)
            {
                p = {{ "y", "" }};
            });

        modify(
            {},
            "y=g",
            [](params_encoded_ref& p)
            {
                p = {{ "y", "g" }};
            });

        modify(
            {},
            "y=g&",
            [](params_encoded_ref& p)
            {
                p = {{ "y", "g" }, {}};
            });

        modify(
            {},
            "y=g&z",
            [](params_encoded_ref& p)
            {
                p = {{ "y", "g" }, { "z" }};
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            {},
            [](params_encoded_ref& p)
            {
                p = {};
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y",
            [](params_encoded_ref& p)
            {
                p = {{ "y" }};
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=",
            [](params_encoded_ref& p)
            {
                p = {{ "y", "" }};
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g",
            [](params_encoded_ref& p)
            {
                p = {{ "y", "g" }};
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&",
            [](params_encoded_ref& p)
            {
                p = {{ "y", "g" }, {}};
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&z",
            [](params_encoded_ref& p)
            {
                p = {{ "y", "g" }, { "z" }};
            });

#if 0
        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "%23&%26==&%3d=%26",
            [](params_encoded_ref& p)
            {
                // encodings
                p = {{ "#" }, { "&", "=" }, { "=", "&" }};
            });
#endif

        //----------------------------------------

        //
        // clear
        //

        modify(
            "",
            {},
            [](params_encoded_ref& p)
            {
                p.clear();
                BOOST_TEST(! p.url().has_query());
            });

        modify(
            "key",
            {},
            [](params_encoded_ref& p)
            {
                p.clear();
                BOOST_TEST(! p.url().has_query());
            });

        modify(
            "key=",
            {},
            [](params_encoded_ref& p)
            {
                p.clear();
                BOOST_TEST(! p.url().has_query());
            });

        modify(
            "key=value",
            {},
            [](params_encoded_ref& p)
            {
                p.clear();
                BOOST_TEST(! p.url().has_query());
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            {},
            [](params_encoded_ref& p)
            {
                p.clear();
                BOOST_TEST(! p.url().has_query());
            });

        //----------------------------------------

        //
        // assign(initializer_list)
        //

        modify(
            {},
            {},
            [](params_encoded_ref& p)
            {
                p.assign({});
            });

        modify(
            {},
            "y",
            [](params_encoded_ref& p)
            {
                p.assign(
                    {{ "y" }});
            });

        modify(
            {},
            "y=",
            [](params_encoded_ref& p)
            {
                p.assign(
                    {{ "y", "" }});
            });

        modify(
            {},
            "y=g",
            [](params_encoded_ref& p)
            {
                p.assign(
                    {{ "y", "g" }});
            });

        modify(
            {},
            "y=g&",
            [](params_encoded_ref& p)
            {
                p.assign(
                    {{ "y", "g" }, {} });
            });

        modify(
            {},
            "y=g&z",
            [](params_encoded_ref& p)
            {
                p.assign(
                    {{ "y", "g" }, { "z" } });
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            {},
            [](params_encoded_ref& p)
            {
                p.assign({});
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y",
            [](params_encoded_ref& p)
            {
                p.assign(
                    {{ "y" }});
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=",
            [](params_encoded_ref& p)
            {
                p.assign(
                    {{ "y", "" }});
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g",
            [](params_encoded_ref& p)
            {
                p.assign(
                    {{ "y", "g" }});
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&",
            [](params_encoded_ref& p)
            {
                p.assign(
                    {{ "y", "g" }, {} });
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&z",
            [](params_encoded_ref& p)
            {
                p.assign(
                    {{ "y", "g" }, { "z" } });
            });

#if 0
        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "%23&%26==&%3d=%26",
            [](params_encoded_ref& p)
            {
                // encodings
                p.assign(
                    {{ "#" }, { "&", "=" }, { "=", "&" }});
            });
#endif

        //
        // assign(FwdIt, FwdIt)
        //

        modify(
            {},
            {},
            [](params_encoded_ref& p)
            {
                p.assign({});
            });

        modify(
            {},
            "y",
            [](params_encoded_ref& p)
            {
                assign(p,
                    {{ "y" }});
            });

        modify(
            {},
            "y=",
            [](params_encoded_ref& p)
            {
                assign(p,
                    {{ "y", "" }});
            });

        modify(
            {},
            "y=g",
            [](params_encoded_ref& p)
            {
                assign(p,
                    {{ "y", "g" }});
            });

        modify(
            {},
            "y=g&",
            [](params_encoded_ref& p)
            {
                assign(p,
                    {{ "y", "g" }, {} });
            });

        modify(
            {},
            "y=g&z",
            [](params_encoded_ref& p)
            {
                assign(p,
                    {{ "y", "g" }, { "z" } });
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            {},
            [](params_encoded_ref& p)
            {
                assign(p, {});
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y",
            [](params_encoded_ref& p)
            {
                assign(p,
                    {{ "y" }});
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=",
            [](params_encoded_ref& p)
            {
                assign(p,
                    {{ "y", "" }});
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g",
            [](params_encoded_ref& p)
            {
                assign(p,
                    {{ "y", "g" }});
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&",
            [](params_encoded_ref& p)
            {
                assign(p,
                    {{ "y", "g" }, {} });
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&z",
            [](params_encoded_ref& p)
            {
                assign(p,
                    {{ "y", "g" }, { "z" } });
            });

#if 0
        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "%23&%26==&%3d=%26",
            [](params_encoded_ref& p)
            {
                // encodings
                assign(p,
                    {{ "#" }, { "&", "=" }, { "=", "&" }});
            });
#endif

        //----------------------------------------

        //
        // append(param_pct_view)
        //

        modify(
            {},
            "y",
            [](params_encoded_ref& p)
            {
                auto it = p.append(
                    { "y" });
                BOOST_TEST(is_equal(
                    *it, { "y" }));
            });

        modify(
            {},
            "y=",
            [](params_encoded_ref& p)
            {
                auto it = p.append(
                    { "y", "" });
                BOOST_TEST(is_equal(
                    *it, { "y", "" }));
            });

        modify(
            {},
            "y=g",
            [](params_encoded_ref& p)
            {
                auto it = p.append(
                    { "y", "g" });
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "",
            "&y",
            [](params_encoded_ref& p)
            {
                auto it = p.append(
                    { "y" });
                BOOST_TEST(is_equal(
                    *it, { "y" }));
            });

        modify(
            "",
            "&y=",
            [](params_encoded_ref& p)
            {
                auto it = p.append(
                    { "y", "" });
                BOOST_TEST(is_equal(
                    *it, { "y", "" }));
            });

        modify(
            "",
            "&y=g",
            [](params_encoded_ref& p)
            {
                auto it = p.append(
                    { "y", "g" });
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "",
            "&key=value",
            [](params_encoded_ref& p)
            {
                // should not go through
                // initializer_list overload
                auto it = p.append({
                    string_view("key"),
                    string_view("value")});
                BOOST_TEST(is_equal(
                    *it, { "key", "value" }));
            });

#if 0
        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=4444&%23",
            [](params_encoded_ref& p)
            {
                // encodings
                p.append({ "#" });
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=4444&%26==",
            [](params_encoded_ref& p)
            {
                // encodings
                p.append({ "&", "=" });
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=4444&%3d=%26",
            [](params_encoded_ref& p)
            {
                // encodings
                p.append({ "=", "&" });
            });
#endif

        //
        // append(FwdIt, FwdIt)
        //

        modify(
            {},
            "y=g&z=q",
            [](params_encoded_ref& p)
            {
                auto it = append(p,
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "",
            "&y=g&z=q",
            [](params_encoded_ref& p)
            {
                auto it = append(p,
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "t",
            "t&y=g&z=q",
            [](params_encoded_ref& p)
            {
                auto it = append(p,
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

#if 0
        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=4444&%23&%26==&%3d=%26",
            [](params_encoded_ref& p)
            {
                // encodings
                append(p,
                    {{ "#" }, { "&", "=" }, { "=", "&" }});
            });
#endif

        //
        // append_list(initializer_list)
        //

        modify(
            {},
            "y=g&z=q",
            [](params_encoded_ref& p)
            {
                auto it = p.append_list(
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "",
            "&y=g&z=q",
            [](params_encoded_ref& p)
            {
                auto it = p.append_list(
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "t",
            "t&y=g&z=q",
            [](params_encoded_ref& p)
            {
                auto it = p.append_list(
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

#if 0
        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=4444&%23&%26==&%3d=%26",
            [](params_encoded_ref& p)
            {
                // encodings
                p.append_list(
                    {{ "#" }, { "&", "=" }, { "=", "&" }});
            });
#endif

        //----------------------------------------

        //
        // insert(iterator, param_pct_view)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&k0=0&k1=1&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.insert(
                    std::next(p.begin(), 0),
                    { "y", "g" });
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&k0=0&k1=1&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.insert(
                    std::next(p.begin(), 0),
                    { "y", "g" });
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&y=g&k1=1&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.insert(
                    std::next(p.begin(), 1),
                    { "y", "g" });
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&y=g&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.insert(
                    std::next(p.begin(), 2),
                    { "y", "g" });
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&y=g&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.insert(
                    std::next(p.begin(), 3),
                    { "y", "g" });
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&y=g&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.insert(
                    std::next(p.begin(), 4),
                    { "y", "g" });
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=4444&y=g",
            [](params_encoded_ref& p)
            {
                auto it = p.insert(
                    std::next(p.begin(), 5),
                    { "y", "g" });
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&key=value&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                // should not go through
                // initializer_list overload
                auto it = p.insert(
                    std::next(p.begin(), 2),
                    {string_view("key"),
                        string_view("value")});
                BOOST_TEST(is_equal(
                    *it, { "key", "value" }));
            });

#if 0
        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&%23&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                // encodings
                p.insert(std::next(p.begin(), 2),
                    { "#" });
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&%26==&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                // encodings
                p.insert(std::next(p.begin(), 2),
                    { "&", "=" });
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&%3d=%26&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                // encodings
                p.insert(std::next(p.begin(), 2),
                    { "=", "&" });
            });
#endif

        //
        // insert(iterator, FwdIt, FwdIt)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&z=q&k0=0&k1=1&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = insert(p,
                    std::next(p.begin(), 0),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&y=g&z=q&k1=1&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = insert(p,
                    std::next(p.begin(), 1),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&y=g&z=q&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = insert(p,
                    std::next(p.begin(), 2),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&y=g&z=q&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = insert(p,
                    std::next(p.begin(), 3),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&y=g&z=q&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = insert(p,
                    std::next(p.begin(), 4),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=4444&y=g&z=q",
            [](params_encoded_ref& p)
            {
                auto it = insert(p,
                    std::next(p.begin(), 5),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

#if 0
        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&%23&%26==&%3d=%26&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                // encodings
                insert(p, std::next(p.begin(), 2),
                    {{ "#" }, { "&", "=" }, { "=", "&" }});
            });
#endif

        //
        // insert_list(iterator, initializer_list)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&z=q&k0=0&k1=1&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.insert_list(
                    std::next(p.begin(), 0),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&y=g&z=q&k1=1&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.insert_list(
                    std::next(p.begin(), 1),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&y=g&z=q&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.insert_list(
                    std::next(p.begin(), 2),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&y=g&z=q&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.insert_list(
                    std::next(p.begin(), 3),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&y=g&z=q&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.insert_list(
                    std::next(p.begin(), 4),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=4444&y=g&z=q",
            [](params_encoded_ref& p)
            {
                auto it = p.insert_list(
                    std::next(p.begin(), 5),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

#if 0
        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&%23&%26==&%3d=%26&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                // encodings
                p.insert_list(std::next(p.begin(), 2),
                    {{ "#" }, { "&", "=" }, { "=", "&" }});
            });
#endif

        //----------------------------------------
        
        //
        // erase(iterator)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k1=1&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.erase(
                    std::next(p.begin(), 0));
                BOOST_TEST(is_equal(
                    *it, { "k1", "1" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.erase(
                    std::next(p.begin(), 1));
                BOOST_TEST(is_equal(
                    *it, { "k2", "" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.erase(
                    std::next(p.begin(), 2));
                BOOST_TEST(is_equal(
                    *it, { "k3" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.erase(
                    std::next(p.begin(), 3));
                BOOST_TEST(is_equal(
                    *it, { "k4", "4444" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3",
            [](params_encoded_ref& p)
            {
                auto it = p.erase(
                    std::next(p.begin(), 4));
                BOOST_TEST_EQ(it, p.end());
            });

        //
        // erase(iterator, iterator)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.erase(
                    std::next(p.begin(), 0),
                    std::next(p.begin(), 2));
                BOOST_TEST(is_equal(
                    *it, { "k2", "" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.erase(
                    std::next(p.begin(), 1),
                    std::next(p.begin(), 3));
                BOOST_TEST(is_equal(
                    *it, { "k3" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.erase(
                    std::next(p.begin(), 2),
                    std::next(p.begin(), 4));
                BOOST_TEST(is_equal(
                    *it, { "k4", "4444" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=",
            [](params_encoded_ref& p)
            {
                auto it = p.erase(
                    std::next(p.begin(), 3),
                    std::next(p.begin(), 5));
                BOOST_TEST_EQ(it, p.end());
            });

        //
        // erase(string_view, ignore_case_param)
        //

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            [](params_encoded_ref& p)
            {
                auto n = p.erase("K0");
                BOOST_TEST_EQ(n, 0);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k1=1&K2=3&k3=4&K2=5&k4=6",
            [](params_encoded_ref& p)
            {
                auto n = p.erase("k0");
                BOOST_TEST_EQ(n, 2);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k1=1&K2=3&k3=4&K2=5&k4=6",
            [](params_encoded_ref& p)
            {
                auto n = p.erase("K0", ignore_case);
                BOOST_TEST_EQ(n, 2);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k0=2&K2=3&k3=4&K2=5&k4=6",
            [](params_encoded_ref& p)
            {
                auto n = p.erase("k1");
                BOOST_TEST_EQ(n, 1);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k0=2&K2=3&k3=4&K2=5&k4=6",
            [](params_encoded_ref& p)
            {
                auto n = p.erase("K1", ignore_case);
                BOOST_TEST_EQ(n, 1);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            [](params_encoded_ref& p)
            {
                auto n = p.erase("k2");
                BOOST_TEST_EQ(n, 0);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&k3=4&k4=6",
            [](params_encoded_ref& p)
            {
                auto n = p.erase("K2");
                BOOST_TEST_EQ(n, 2);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&k3=4&k4=6",
            [](params_encoded_ref& p)
            {
                auto n = p.erase("k2", ignore_case);
                BOOST_TEST_EQ(n, 2);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&K2=3&K2=5&k4=6",
            [](params_encoded_ref& p)
            {
                auto n = p.erase("k3");
                BOOST_TEST_EQ(n, 1);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5",
            [](params_encoded_ref& p)
            {
                auto n = p.erase("k4");
                BOOST_TEST_EQ(n, 1);
            });

        //----------------------------------------

        //
        // replace(iterator, param_pct_view)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&k1=1&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.replace(
                    p.find("k0"), { "y", "g" });
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&y=g&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.replace(
                    p.find("k1"), { "y", "g" });
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&y=g&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.replace(
                    p.find("k2"), { "y", "g" });
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&y=g&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.replace(
                    p.find("k3"), { "y", "g" });
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&y=g",
            [](params_encoded_ref& p)
            {
                auto it = p.replace(
                    p.find("k4"), { "y", "g" });
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

#if 0
        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&%23&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                // encodings
                p.replace(p.find("k1"),
                    { "#" });
            });
#endif

        //
        // replace(iterator, iterator, initializer_list)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&z=q&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.replace(
                    p.find("k0"), p.find("k3"),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&y=g&z=q&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.replace(
                    p.find("k1"), p.find("k4"),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&y=g&z=q",
            [](params_encoded_ref& p)
            {
                auto it = p.replace(
                    p.find("k2"), p.end(),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            {},
            [](params_encoded_ref& p)
            {
                // clear
                auto it = p.replace(
                    p.find("k0"), p.end(), {});
                BOOST_TEST_EQ(it, p.begin());
                BOOST_TEST_EQ(it, p.end());
            });

#if 0
        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&%23&%26==&%3d=%26&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                // encodings
                p.replace(
                    p.find("k1"), p.find("k3"),
                    {{ "#" }, { "&", "=" }, { "=", "&" }});
            });
#endif

        //
        // replace(iterator, iterator, FwdIt, FwdIt)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&z=q&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = replace(p,
                    p.find("k0"), p.find("k3"),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&y=g&z=q&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = replace(p,
                    p.find("k1"), p.find("k4"),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&y=g&z=q",
            [](params_encoded_ref& p)
            {
                auto it = replace(p,
                    p.find("k2"), p.end(),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            {},
            [](params_encoded_ref& p)
            {
                // clear
                auto it = replace(p,
                    p.find("k0"), p.end(), {});
                BOOST_TEST_EQ(it, p.begin());
                BOOST_TEST_EQ(it, p.end());
            });

#if 0
        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&%23&%26==&%3d=%26&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                // encodings
                replace(p,
                    p.find("k1"), p.find("k3"),
                    {{ "#" }, { "&", "=" }, { "=", "&" }});
            });
#endif

        //----------------------------------------

        //
        // unset(iterator)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0&k1=1&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.unset(next(p.begin(), 0));
                BOOST_TEST(is_equal(*it, { "k0" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.unset(next(p.begin(), 1));
                BOOST_TEST(is_equal(*it, { "k1" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.unset(next(p.begin(), 2));
                BOOST_TEST(is_equal(*it, { "k2" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.unset(next(p.begin(), 3));
                BOOST_TEST(is_equal(*it, { "k3" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4",
            [](params_encoded_ref& p)
            {
                auto it = p.unset(next(p.begin(), 4));
                BOOST_TEST(is_equal(*it, { "k4" }));
            });

        //
        // set(iterator, string_view)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=42&k1=1&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.set(next(p.begin(), 0), "42");
                BOOST_TEST(is_equal(*it, { "k0", "42" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=42&k2=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.set(next(p.begin(), 1), "42");
                BOOST_TEST(is_equal(*it, { "k1", "42" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=42&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.set(next(p.begin(), 2), "42");
                BOOST_TEST(is_equal(*it, { "k2", "42" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3=42&k4=4444",
            [](params_encoded_ref& p)
            {
                auto it = p.set(next(p.begin(), 3), "42");
                BOOST_TEST(is_equal(*it, { "k3", "42" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=42",
            [](params_encoded_ref& p)
            {
                auto it = p.set(next(p.begin(), 4), "42");
                BOOST_TEST(is_equal(*it, { "k4", "42" }));
            });

#if 0
        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=%23%25%26=&k3&k4=4444",
            [](params_encoded_ref& p)
            {
                // encodings
                p.set(next(p.begin(), 2), "#%&=");
            });
#endif

        //
        // set(string_view, string_view, ignore_case_param)
        //

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=x&k1=1&K2=3&k3=4&K2=5&k4=6",
            [](params_encoded_ref& p)
            {
                auto it = p.set("k0", "x");
                BOOST_TEST(is_equal(*it, { "k0", "x" }));
                BOOST_TEST_EQ(p.count("k0"), 1);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=x&k0=2&K2=3&k3=4&K2=5&k4=6",
            [](params_encoded_ref& p)
            {
                auto it = p.set("k1", "x");
                BOOST_TEST(is_equal(*it, { "k1", "x" }));
                BOOST_TEST_EQ(p.count("k1"), 1);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6&k2=x",
            [](params_encoded_ref& p)
            {
                auto it = p.set("k2", "x");
                BOOST_TEST(is_equal(*it, { "k2", "x" }));
                BOOST_TEST_EQ(p.count("k2"), 1);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&K2=x&k3=4&k4=6",
            [](params_encoded_ref& p)
            {
                auto it = p.set("k2", "x", ignore_case);
                BOOST_TEST(is_equal(*it, { "K2", "x" }));
                BOOST_TEST_EQ(p.count("k2", ignore_case), 1);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&K2=3&k3=x&K2=5&k4=6",
            [](params_encoded_ref& p)
            {
                auto it = p.set("k3", "x");
                BOOST_TEST(is_equal(*it, { "k3", "x" }));
                BOOST_TEST_EQ(p.count("k3"), 1);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=x",
            [](params_encoded_ref& p)
            {
                auto it = p.set("k4", "x");
                BOOST_TEST(is_equal(*it, { "k4", "x" }));
                BOOST_TEST_EQ(p.count("k4"), 1);
            });

#if 0
        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&K2=%23%25%26=&k3=4&k4=6",
            [](params_encoded_ref& p)
            {
                // encodings
                p.set("k2", "#%&=", ignore_case);
            });
#endif
    }

    void
    testIterator()
    {
        using T = params_encoded_ref::iterator;

        // iterator()
        {
            T t0;
            T t1;
            BOOST_TEST_EQ(t0, t1);
        }

        // operator==()
        {
            url u;
            BOOST_TEST_EQ(
                u.encoded_params().begin(),
                u.encoded_params().begin());
        }

        // operator!=()
        {
            url u("?");
            BOOST_TEST_NE(
                u.encoded_params().begin(),
                u.encoded_params().end());
        }

        // value_type outlives reference
        {
            params_encoded_ref::value_type v;
            {
                url u("/?a=1&bb=22&ccc=333&dddd=4444#f");
                params_encoded_ref ps = u.encoded_params();
                params_encoded_ref::reference r = *ps.begin();
                v = params_encoded_ref::value_type(r);
            }
            BOOST_TEST_EQ(v.key, "a");
            BOOST_TEST_EQ(v.value, "1");
            BOOST_TEST_EQ(v.has_value, true);
        }
    }

    void
    testJavadocs()
    {
        // url()
        {
        url u( "?key=value" );

        assert( &u.encoded_params().url() == &u );
        }
    }
    void
    run()
    {
        testSpecial();
        testModifiers();
        testIterator();
        testJavadocs();
    }
};

TEST_SUITE(
    params_encoded_ref_test,
    "boost.url.params_encoded_ref");

} // urls
} // boost
