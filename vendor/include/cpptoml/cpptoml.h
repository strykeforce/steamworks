/**
 * @file cpptoml.h
 * @author Chase Geigle
 * @date May 2013
 */

#ifndef _CPPTOML_H_
#define _CPPTOML_H_

#include <algorithm>
#if !CPPTOML_HAS_STD_PUT_TIME
#include <array>
#endif
#include <cassert>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <memory>
#if CPPTOML_HAS_STD_REGEX
#include <regex>
#endif
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace cpptoml
{
class writer; // forward declaration
class base;   // forward declaration
#if defined(CPPTOML_USE_MAP)
// a std::map will ensure that entries a sorted, albeit at a slight
// performance penalty relative to the (default) unordered_map
using string_to_base_map = std::map<std::string, std::shared_ptr<base>>;
#else
// by default an unordered_map is used for best performance as the
// toml specification does not require entries to be sorted
using string_to_base_map
    = std::unordered_map<std::string, std::shared_ptr<base>>;
#endif

template <class T>
class option
{
  public:
    option() : empty_{true}
    {
        // nothing
    }

    option(T value) : empty_{false}, value_{std::move(value)}
    {
        // nothing
    }

    explicit operator bool() const
    {
        return !empty_;
    }

    const T& operator*() const
    {
        return value_;
    }

    const T* operator->() const
    {
        return &value_;
    }

    const T& value_or(const T& alternative) const
    {
        if (!empty_)
            return value_;
        return alternative;
    }

  private:
    bool empty_;
    T value_;
};

struct datetime
{
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    int microsecond = 0;
    int hour_offset = 0;
    int minute_offset = 0;

    static inline struct datetime from_local(const struct tm& t)
    {
        datetime dt;
        dt.year = t.tm_year + 1900;
        dt.month = t.tm_mon + 1;
        dt.day = t.tm_mday;
        dt.hour = t.tm_hour;
        dt.minute = t.tm_min;
        dt.second = t.tm_sec;

        char buf[16];
        strftime(buf, 16, "%z", &t);

        int offset = std::stoi(buf);
        dt.hour_offset = offset / 100;
        dt.minute_offset = offset % 100;
        return dt;
    }

    static inline struct datetime from_utc(const struct tm& t)
    {
        datetime dt;
        dt.year = t.tm_year + 1900;
        dt.month = t.tm_mon + 1;
        dt.day = t.tm_mday;
        dt.hour = t.tm_hour;
        dt.minute = t.tm_min;
        dt.second = t.tm_sec;
        return dt;
    }
};

inline std::ostream& operator<<(std::ostream& os, const datetime& dt)
{
    using std::setw;
    auto fill = os.fill();

    os.fill('0');
    os << setw(4) << dt.year << "-" << setw(2) << dt.month << "-" << setw(2)
       << dt.day << "T" << setw(2) << dt.hour << ":" << setw(2) << dt.minute
       << ":" << setw(2) << dt.second;

    if (dt.microsecond > 0)
    {
        os << "." << setw(6) << dt.microsecond;
    }

    if (dt.hour_offset != 0 || dt.minute_offset != 0)
    {
        if (dt.hour_offset > 0)
            os << "+";
        else
            os << "-";
        os << setw(2) << std::abs(dt.hour_offset) << ":" << setw(2)
           << std::abs(dt.minute_offset);
    }
    else
        os << "Z";

    os.fill(fill);

    return os;
}

template <class T>
class value;

template <class T>
struct valid_value
{
    const static bool value
        = std::is_same<T, std::string>::value || std::is_same<T, int64_t>::value
          || std::is_same<T, double>::value || std::is_same<T, bool>::value
          || std::is_same<T, datetime>::value;
};

template <class T, class Enable = void>
struct value_traits;

template <class T>
struct valid_value_or_string_convertible
{

    const static bool value = valid_value<typename std::decay<T>::type>::value
                              || std::is_convertible<T, std::string>::value;
};

template <class T>
struct value_traits<T, typename std::
                           enable_if<valid_value_or_string_convertible<T>::
                                         value>::type>
{
    using value_type = typename std::
        conditional<valid_value<typename std::decay<T>::type>::value,
                    typename std::decay<T>::type, std::string>::type;

    using type = value<value_type>;

    static value_type construct(T&& val)
    {
        return value_type{val};
    }
};

template <class T>
struct value_traits<T,
                    typename std::
                        enable_if<!valid_value_or_string_convertible<T>::value
                                  && std::is_floating_point<
                                         typename std::decay<T>::type>::value>::
                            type>
{
    using value_type = typename std::decay<T>::type;

    using type = value<double>;

    static value_type construct(T&& val)
    {
        return value_type{val};
    }
};

template <class T>
struct value_traits<T,
                    typename std::
                        enable_if<!valid_value_or_string_convertible<T>::value
                                  && std::is_signed<typename std::decay<T>::
                                                        type>::value>::type>
{
    using value_type = int64_t;

    using type = value<int64_t>;

    static value_type construct(T&& val)
    {
        if (val < std::numeric_limits<int64_t>::min())
            throw std::underflow_error{"constructed value cannot be "
                                       "represented by a 64-bit signed "
                                       "integer"};

        if (val > std::numeric_limits<int64_t>::max())
            throw std::overflow_error{"constructed value cannot be represented "
                                      "by a 64-bit signed integer"};

        return static_cast<int64_t>(val);
    }
};

template <class T>
struct value_traits<T,
                    typename std::
                        enable_if<!valid_value_or_string_convertible<T>::value
                                  && std::is_unsigned<typename std::decay<T>::
                                                          type>::value>::type>
{
    using value_type = int64_t;

    using type = value<int64_t>;

    static value_type construct(T&& val)
    {
        if (val > static_cast<uint64_t>(std::numeric_limits<int64_t>::max()))
            throw std::overflow_error{"constructed value cannot be represented "
                                      "by a 64-bit signed integer"};

        return static_cast<int64_t>(val);
    }
};

class array;
class table;
class table_array;

template <class T>
struct array_of_trait
{
    using return_type = option<std::vector<T>>;
};

template <>
struct array_of_trait<array>
{
    using return_type = option<std::vector<std::shared_ptr<array>>>;
};

template <class T>
inline std::shared_ptr<typename value_traits<T>::type> make_value(T&& val);
inline std::shared_ptr<array> make_array();
template <class T>
inline std::shared_ptr<T> make_element();
inline std::shared_ptr<table> make_table();
inline std::shared_ptr<table_array> make_table_array();

/**
 * A generic base TOML value used for type erasure.
 */
class base : public std::enable_shared_from_this<base>
{
  public:
    /**
     * Determines if the given TOML element is a value.
     */
    virtual bool is_value() const
    {
        return false;
    }

    /**
     * Determines if the given TOML element is a table.
     */
    virtual bool is_table() const
    {
        return false;
    }

    /**
     * Converts the TOML element into a table.
     */
    std::shared_ptr<table> as_table()
    {
        if (is_table())
            return std::static_pointer_cast<table>(shared_from_this());
        return nullptr;
    }
    /**
     * Determines if the TOML element is an array of "leaf" elements.
     */
    virtual bool is_array() const
    {
        return false;
    }

    /**
     * Converts the TOML element to an array.
     */
    std::shared_ptr<array> as_array()
    {
        if (is_array())
            return std::static_pointer_cast<array>(shared_from_this());
        return nullptr;
    }

    /**
     * Determines if the given TOML element is an array of tables.
     */
    virtual bool is_table_array() const
    {
        return false;
    }

    /**
     * Converts the TOML element into a table array.
     */
    std::shared_ptr<table_array> as_table_array()
    {
        if (is_table_array())
            return std::static_pointer_cast<table_array>(shared_from_this());
        return nullptr;
    }

    /**
     * Attempts to coerce the TOML element into a concrete TOML value
     * of type T.
     */
    template <class T>
    std::shared_ptr<value<T>> as();

    template <class T>
    std::shared_ptr<const value<T>> as() const;

    template <class Visitor, class... Args>
    void accept(Visitor&& visitor, Args&&... args) const;

  protected:
    base()
    {
        // nothing
    }
};

/**
 * A concrete TOML value representing the "leaves" of the "tree".
 */
template <class T>
class value : public base
{
    struct make_shared_enabler
    {
        // nothing; this is a private key accessible only to friends
    };

    template <class U>
    friend std::shared_ptr<typename value_traits<U>::type>
    cpptoml::make_value(U&& val);

  public:
    static_assert(valid_value<T>::value, "invalid value type");

    value(const make_shared_enabler&, const T& val) : value(val)
    {
        // nothing; note that users cannot actually invoke this function
        // because they lack access to the make_shared_enabler.
    }

    bool is_value() const override
    {
        return true;
    }

    /**
     * Gets the data associated with this value.
     */
    T& get()
    {
        return data_;
    }

    /**
     * Gets the data associated with this value. Const version.
     */
    const T& get() const
    {
        return data_;
    }

  private:
    T data_;

    /**
     * Constructs a value from the given data.
     */
    value(const T& val) : data_(val)
    {
    }

    value(const value& val) = delete;
    value& operator=(const value& val) = delete;
};

template <class T>
std::shared_ptr<typename value_traits<T>::type> make_value(T&& val)
{
    using value_type = typename value_traits<T>::type;
    using enabler = typename value_type::make_shared_enabler;
    return std::make_shared<value_type>(
        enabler{}, value_traits<T>::construct(std::forward<T>(val)));
}

template <class T>
inline std::shared_ptr<value<T>> base::as()
{
    return std::dynamic_pointer_cast<value<T>>(shared_from_this());
}

// special case value<double> to allow getting an integer parameter as a
// double value
template <>
inline std::shared_ptr<value<double>> base::as()
{
    if (auto v = std::dynamic_pointer_cast<value<double>>(shared_from_this()))
        return v;

    if (auto v = std::dynamic_pointer_cast<value<int64_t>>(shared_from_this()))
        return make_value<double>(static_cast<double>(v->get()));

    return nullptr;
}

template <class T>
inline std::shared_ptr<const value<T>> base::as() const
{
    return std::dynamic_pointer_cast<const value<T>>(shared_from_this());
}

// special case value<double> to allow getting an integer parameter as a
// double value
template <>
inline std::shared_ptr<const value<double>> base::as() const
{
    if (auto v
        = std::dynamic_pointer_cast<const value<double>>(shared_from_this()))
        return v;

    if (auto v = as<int64_t>())
    {
        // the below has to be a non-const value<double> due to a bug in
        // libc++: https://llvm.org/bugs/show_bug.cgi?id=18843
        return make_value<double>(static_cast<double>(v->get()));
    }

    return nullptr;
}

/**
 * Exception class for array insertion errors.
 */
class array_exception : public std::runtime_error
{
  public:
    array_exception(const std::string& err) : std::runtime_error{err}
    {
    }
};

class array : public base
{
  public:
    friend std::shared_ptr<array> make_array();

    virtual bool is_array() const override
    {
        return true;
    }

    /**
     * arrays can be iterated over
     */
    using iterator = std::vector<std::shared_ptr<base>>::iterator;

    /**
     * arrays can be iterated over.  Const version.
     */
    using const_iterator = std::vector<std::shared_ptr<base>>::const_iterator;

    iterator begin()
    {
        return values_.begin();
    }

    const_iterator begin() const
    {
        return values_.begin();
    }

    iterator end()
    {
        return values_.end();
    }

    const_iterator end() const
    {
        return values_.end();
    }

    /**
     * Obtains the array (vector) of base values.
     */
    std::vector<std::shared_ptr<base>>& get()
    {
        return values_;
    }

    /**
     * Obtains the array (vector) of base values. Const version.
     */
    const std::vector<std::shared_ptr<base>>& get() const
    {
        return values_;
    }

    std::shared_ptr<base> at(size_t idx) const
    {
        return values_.at(idx);
    }

    /**
     * Obtains an array of value<T>s. Note that elements may be
     * nullptr if they cannot be converted to a value<T>.
     */
    template <class T>
    std::vector<std::shared_ptr<value<T>>> array_of() const
    {
        std::vector<std::shared_ptr<value<T>>> result(values_.size());

        std::transform(values_.begin(), values_.end(), result.begin(),
                       [&](std::shared_ptr<base> v) { return v->as<T>(); });

        return result;
    }

    /**
     * Obtains a option<vector<T>>. The option will be empty if the array
     * contains values that are not of type T.
     */
    template <class T>
    inline typename array_of_trait<T>::return_type get_array_of() const
    {
        std::vector<T> result;
        result.reserve(values_.size());

        for (const auto& val : values_)
        {
            if (auto v = val->as<T>())
                result.push_back(v->get());
            else
                return {};
        }

        return {std::move(result)};
    }

    /**
     * Obtains an array of arrays. Note that elements may be nullptr
     * if they cannot be converted to a array.
     */
    std::vector<std::shared_ptr<array>> nested_array() const
    {
        std::vector<std::shared_ptr<array>> result(values_.size());

        std::transform(values_.begin(), values_.end(), result.begin(),
                       [&](std::shared_ptr<base> v) -> std::shared_ptr<array> {
                           if (v->is_array())
                               return std::static_pointer_cast<array>(v);
                           return std::shared_ptr<array>{};
                       });

        return result;
    }

    /**
     * Add a value to the end of the array
     */
    template <class T>
    void push_back(const std::shared_ptr<value<T>>& val)
    {
        if (values_.empty() || values_[0]->as<T>())
        {
            values_.push_back(val);
        }
        else
        {
            throw array_exception{"Arrays must be homogenous."};
        }
    }

    /**
     * Add an array to the end of the array
     */
    void push_back(const std::shared_ptr<array>& val)
    {
        if (values_.empty() || values_[0]->is_array())
        {
            values_.push_back(val);
        }
        else
        {
            throw array_exception{"Arrays must be homogenous."};
        }
    }

    /**
     * Convenience function for adding a simple element to the end
     * of the array.
     */
    template <class T>
    void push_back(T&& val, typename value_traits<T>::type* = 0)
    {
        push_back(make_value(std::forward<T>(val)));
    }

    /**
     * Insert a value into the array
     */
    template <class T>
    iterator insert(iterator position, const std::shared_ptr<value<T>>& value)
    {
        if (values_.empty() || values_[0]->as<T>())
        {
            return values_.insert(position, value);
        }
        else
        {
            throw array_exception{"Arrays must be homogenous."};
        }
    }

    /**
     * Insert an array into the array
     */
    iterator insert(iterator position, const std::shared_ptr<array>& value)
    {
        if (values_.empty() || values_[0]->is_array())
        {
            return values_.insert(position, value);
        }
        else
        {
            throw array_exception{"Arrays must be homogenous."};
        }
    }

    /**
     * Convenience function for inserting a simple element in the array
     */
    template <class T>
    iterator insert(iterator position, T&& val,
                    typename value_traits<T>::type* = 0)
    {
        return insert(position, make_value(std::forward<T>(val)));
    }

    /**
     * Erase an element from the array
     */
    iterator erase(iterator position)
    {
        return values_.erase(position);
    }

    /**
     * Clear the array
     */
    void clear()
    {
        values_.clear();
    }

  private:
    array() = default;

    template <class InputIterator>
    array(InputIterator begin, InputIterator end) : values_{begin, end}
    {
        // nothing
    }

    array(const array& obj) = delete;
    array& operator=(const array& obj) = delete;

    std::vector<std::shared_ptr<base>> values_;
};

inline std::shared_ptr<array> make_array()
{
    struct make_shared_enabler : public array
    {
        make_shared_enabler()
        {
            // nothing
        }
    };

    return std::make_shared<make_shared_enabler>();
}

template <>
inline std::shared_ptr<array> make_element<array>()
{
    return make_array();
}

/**
 * Obtains a option<vector<T>>. The option will be empty if the array
 * contains values that are not of type T.
 */
template <>
inline typename array_of_trait<array>::return_type
array::get_array_of<array>() const
{
    std::vector<std::shared_ptr<array>> result;
    result.reserve(values_.size());

    for (const auto& val : values_)
    {
        if (auto v = val->as_array())
            result.push_back(v);
        else
            return {};
    }

    return {std::move(result)};
}

class table;

class table_array : public base
{
    friend class table;
    friend std::shared_ptr<table_array> make_table_array();

  public:
    /**
     * arrays can be iterated over
     */
    using iterator = std::vector<std::shared_ptr<table>>::iterator;

    /**
     * arrays can be iterated over.  Const version.
     */
    using const_iterator = std::vector<std::shared_ptr<table>>::const_iterator;

    iterator begin()
    {
        return array_.begin();
    }

    const_iterator begin() const
    {
        return array_.begin();
    }

    iterator end()
    {
        return array_.end();
    }

    const_iterator end() const
    {
        return array_.end();
    }

    virtual bool is_table_array() const override
    {
        return true;
    }

    std::vector<std::shared_ptr<table>>& get()
    {
        return array_;
    }

    const std::vector<std::shared_ptr<table>>& get() const
    {
        return array_;
    }

    /**
     * Add a table to the end of the array
     */
    void push_back(const std::shared_ptr<table>& val)
    {
        array_.push_back(val);
    }

    /**
     * Insert a table into the array
     */
    iterator insert(iterator position, const std::shared_ptr<table>& value)
    {
        return array_.insert(position, value);
    }

    /**
     * Erase an element from the array
     */
    iterator erase(iterator position)
    {
        return array_.erase(position);
    }

    /**
     * Clear the array
     */
    void clear()
    {
        array_.clear();
    }

  private:
    table_array()
    {
        // nothing
    }

    table_array(const table_array& obj) = delete;
    table_array& operator=(const table_array& rhs) = delete;

    std::vector<std::shared_ptr<table>> array_;
};

inline std::shared_ptr<table_array> make_table_array()
{
    struct make_shared_enabler : public table_array
    {
        make_shared_enabler()
        {
            // nothing
        }
    };

    return std::make_shared<make_shared_enabler>();
}

template <>
inline std::shared_ptr<table_array> make_element<table_array>()
{
    return make_table_array();
}

// The below are overloads for fetching specific value types out of a value
// where special casting behavior (like bounds checking) is desired

template <class T>
typename std::enable_if<std::is_signed<T>::value, option<T>>::type
get_impl(const std::shared_ptr<base>& elem)
{
    if (auto v = elem->as<int64_t>())
    {
        if (v->get() < std::numeric_limits<T>::min())
            throw std::underflow_error{
                "T cannot represent the value requested in get"};

        if (v->get() > std::numeric_limits<T>::max())
            throw std::overflow_error{
                "T cannot represent the value requested in get"};

        return {static_cast<T>(v->get())};
    }
    else
    {
        return {};
    }
}

template <class T>
typename std::enable_if<!std::is_same<T, bool>::value
                            && std::is_unsigned<T>::value,
                        option<T>>::type
get_impl(const std::shared_ptr<base>& elem)
{
    if (auto v = elem->as<int64_t>())
    {
        if (v->get() < 0)
            throw std::underflow_error{"T cannot store negative value in get"};

        if (static_cast<uint64_t>(v->get()) > std::numeric_limits<T>::max())
            throw std::overflow_error{
                "T cannot represent the value requested in get"};

        return {static_cast<T>(v->get())};
    }
    else
    {
        return {};
    }
}

template <class T>
typename std::enable_if<!std::is_integral<T>::value
                            || std::is_same<T, bool>::value,
                        option<T>>::type
get_impl(const std::shared_ptr<base>& elem)
{
    if (auto v = elem->as<T>())
    {
        return {v->get()};
    }
    else
    {
        return {};
    }
}

/**
 * Represents a TOML keytable.
 */
class table : public base
{
  public:
    friend class table_array;
    friend std::shared_ptr<table> make_table();

    /**
     * tables can be iterated over.
     */
    using iterator = string_to_base_map::iterator;

    /**
     * tables can be iterated over. Const version.
     */
    using const_iterator = string_to_base_map::const_iterator;

    iterator begin()
    {
        return map_.begin();
    }

    const_iterator begin() const
    {
        return map_.begin();
    }

    iterator end()
    {
        return map_.end();
    }

    const_iterator end() const
    {
        return map_.end();
    }

    bool is_table() const override
    {
        return true;
    }

    bool empty() const
    {
        return map_.empty();
    }

    /**
     * Determines if this key table contains the given key.
     */
    bool contains(const std::string& key) const
    {
        return map_.find(key) != map_.end();
    }

    /**
     * Determines if this key table contains the given key. Will
     * resolve "qualified keys". Qualified keys are the full access
     * path separated with dots like "grandparent.parent.child".
     */
    bool contains_qualified(const std::string& key) const
    {
        return resolve_qualified(key);
    }

    /**
     * Obtains the base for a given key.
     * @throw std::out_of_range if the key does not exist
     */
    std::shared_ptr<base> get(const std::string& key) const
    {
        return map_.at(key);
    }

    /**
     * Obtains the base for a given key. Will resolve "qualified
     * keys". Qualified keys are the full access path separated with
     * dots like "grandparent.parent.child".
     *
     * @throw std::out_of_range if the key does not exist
     */
    std::shared_ptr<base> get_qualified(const std::string& key) const
    {
        std::shared_ptr<base> p;
        resolve_qualified(key, &p);
        return p;
    }

    /**
     * Obtains a table for a given key, if possible.
     */
    std::shared_ptr<table> get_table(const std::string& key) const
    {
        if (contains(key) && get(key)->is_table())
            return std::static_pointer_cast<table>(get(key));
        return nullptr;
    }

    /**
     * Obtains a table for a given key, if possible. Will resolve
     * "qualified keys".
     */
    std::shared_ptr<table> get_table_qualified(const std::string& key) const
    {
        if (contains_qualified(key) && get_qualified(key)->is_table())
            return std::static_pointer_cast<table>(get_qualified(key));
        return nullptr;
    }

    /**
     * Obtains an array for a given key.
     */
    std::shared_ptr<array> get_array(const std::string& key) const
    {
        if (!contains(key))
            return nullptr;
        return get(key)->as_array();
    }

    /**
     * Obtains an array for a given key. Will resolve "qualified keys".
     */
    std::shared_ptr<array> get_array_qualified(const std::string& key) const
    {
        if (!contains_qualified(key))
            return nullptr;
        return get_qualified(key)->as_array();
    }

    /**
     * Obtains a table_array for a given key, if possible.
     */
    std::shared_ptr<table_array> get_table_array(const std::string& key) const
    {
        if (!contains(key))
            return nullptr;
        return get(key)->as_table_array();
    }

    /**
     * Obtains a table_array for a given key, if possible. Will resolve
     * "qualified keys".
     */
    std::shared_ptr<table_array>
    get_table_array_qualified(const std::string& key) const
    {
        if (!contains_qualified(key))
            return nullptr;
        return get_qualified(key)->as_table_array();
    }

    /**
     * Helper function that attempts to get a value corresponding
     * to the template parameter from a given key.
     */
    template <class T>
    option<T> get_as(const std::string& key) const
    {
        try
        {
            return get_impl<T>(get(key));
        }
        catch (const std::out_of_range&)
        {
            return {};
        }
    }

    /**
     * Helper function that attempts to get a value corresponding
     * to the template parameter from a given key. Will resolve "qualified
     * keys".
     */
    template <class T>
    option<T> get_qualified_as(const std::string& key) const
    {
        try
        {
            return get_impl<T>(get_qualified(key));
        }
        catch (const std::out_of_range&)
        {
            return {};
        }
    }

    /**
     * Helper function that attempts to get an array of values of a given
     * type corresponding to the template parameter for a given key.
     *
     * If the key doesn't exist, doesn't exist as an array type, or one or
     * more keys inside the array type are not of type T, an empty option
     * is returned. Otherwise, an option containing a vector of the values
     * is returned.
     */
    template <class T>
    inline typename array_of_trait<T>::return_type
    get_array_of(const std::string& key) const
    {
        if (auto v = get_array(key))
        {
            std::vector<T> result;
            result.reserve(v->get().size());

            for (const auto& b : v->get())
            {
                if (auto val = b->as<T>())
                    result.push_back(val->get());
                else
                    return {};
            }
            return {std::move(result)};
        }

        return {};
    }

    /**
     * Helper function that attempts to get an array of values of a given
     * type corresponding to the template parameter for a given key. Will
     * resolve "qualified keys".
     *
     * If the key doesn't exist, doesn't exist as an array type, or one or
     * more keys inside the array type are not of type T, an empty option
     * is returned. Otherwise, an option containing a vector of the values
     * is returned.
     */
    template <class T>
    inline typename array_of_trait<T>::return_type
    get_qualified_array_of(const std::string& key) const
    {
        if (auto v = get_array_qualified(key))
        {
            std::vector<T> result;
            result.reserve(v->get().size());

            for (const auto& b : v->get())
            {
                if (auto val = b->as<T>())
                    result.push_back(val->get());
                else
                    return {};
            }
            return {std::move(result)};
        }

        return {};
    }

    /**
     * Adds an element to the keytable.
     */
    void insert(const std::string& key, const std::shared_ptr<base>& value)
    {
        map_[key] = value;
    }

    /**
     * Convenience shorthand for adding a simple element to the
     * keytable.
     */
    template <class T>
    void insert(const std::string& key, T&& val,
                typename value_traits<T>::type* = 0)
    {
        insert(key, make_value(std::forward<T>(val)));
    }

    /**
     * Removes an element from the table.
     */
    void erase(const std::string& key)
    {
        map_.erase(key);
    }

  private:
    table()
    {
        // nothing
    }

    table(const table& obj) = delete;
    table& operator=(const table& rhs) = delete;

    std::vector<std::string> split(const std::string& value,
                                   char separator) const
    {
        std::vector<std::string> result;
        std::string::size_type p = 0;
        std::string::size_type q;
        while ((q = value.find(separator, p)) != std::string::npos)
        {
            result.emplace_back(value, p, q - p);
            p = q + 1;
        }
        result.emplace_back(value, p);
        return result;
    }

    // If output parameter p is specified, fill it with the pointer to the
    // specified entry and throw std::out_of_range if it couldn't be found.
    //
    // Otherwise, just return true if the entry could be found or false
    // otherwise and do not throw.
    bool resolve_qualified(const std::string& key,
                           std::shared_ptr<base>* p = nullptr) const
    {
        auto parts = split(key, '.');
        auto last_key = parts.back();
        parts.pop_back();

        auto table = this;
        for (const auto& part : parts)
        {
            table = table->get_table(part).get();
            if (!table)
            {
                if (!p)
                    return false;

                throw std::out_of_range{key + " is not a valid key"};
            }
        }

        if (!p)
            return table->map_.count(last_key) != 0;

        *p = table->map_.at(last_key);
        return true;
    }

    string_to_base_map map_;
};

/**
 * Helper function that attempts to get an array of arrays for a given
 * key.
 *
 * If the key doesn't exist, doesn't exist as an array type, or one or
 * more keys inside the array type are not of type T, an empty option
 * is returned. Otherwise, an option containing a vector of the values
 * is returned.
 */
template <>
inline typename array_of_trait<array>::return_type
table::get_array_of<array>(const std::string& key) const
{
    if (auto v = get_array(key))
    {
        std::vector<std::shared_ptr<array>> result;
        result.reserve(v->get().size());

        for (const auto& b : v->get())
        {
            if (auto val = b->as_array())
                result.push_back(val);
            else
                return {};
        }

        return {std::move(result)};
    }

    return {};
}

/**
 * Helper function that attempts to get an array of arrays for a given
 * key. Will resolve "qualified keys".
 *
 * If the key doesn't exist, doesn't exist as an array type, or one or
 * more keys inside the array type are not of type T, an empty option
 * is returned. Otherwise, an option containing a vector of the values
 * is returned.
 */
template <>
inline typename array_of_trait<array>::return_type
table::get_qualified_array_of<array>(const std::string& key) const
{
    if (auto v = get_array(key))
    {
        std::vector<std::shared_ptr<array>> result;
        result.reserve(v->get().size());

        for (const auto& b : v->get())
        {
            if (auto val = b->as_array())
                result.push_back(val);
            else
                return {};
        }

        return {std::move(result)};
    }

    return {};
}

std::shared_ptr<table> make_table()
{
    struct make_shared_enabler : public table
    {
        make_shared_enabler()
        {
            // nothing
        }
    };

    return std::make_shared<make_shared_enabler>();
}

template <>
inline std::shared_ptr<table> make_element<table>()
{
    return make_table();
}

/**
 * Exception class for all TOML parsing errors.
 */
class parse_exception : public std::runtime_error
{
  public:
    parse_exception(const std::string& err) : std::runtime_error{err}
    {
    }

    parse_exception(const std::string& err, std::size_t line_number)
        : std::runtime_error{err + " at line " + std::to_string(line_number)}
    {
    }
};

/**
 * The parser class.
 */
class parser
{
  public:
    /**
     * Parsers are constructed from streams.
     */
    parser(std::istream& stream) : input_(stream)
    {
    }

    parser& operator=(const parser& parser) = delete;

    /**
     * Parses the stream this parser was created on until EOF.
     * @throw parse_exception if there are errors in parsing
     */
    std::shared_ptr<table> parse()
    {
        std::shared_ptr<table> root = make_table();

        table* curr_table = root.get();

        while (std::getline(input_, line_))
        {
            line_number_++;
            auto it = line_.begin();
            auto end = line_.end();
            consume_whitespace(it, end);
            if (it == end || *it == '#')
                continue;
            if (*it == '[')
            {
                curr_table = root.get();
                parse_table(it, end, curr_table);
            }
            else
            {
                parse_key_value(it, end, curr_table);
                consume_whitespace(it, end);
                eol_or_comment(it, end);
            }
        }
        return root;
    }

  private:
#if defined _MSC_VER
    __declspec(noreturn)
#elif defined __GNUC__
    __attribute__((noreturn))
#endif
        void throw_parse_exception(const std::string& err)
    {
        throw parse_exception{err, line_number_};
    }

    void parse_table(std::string::iterator& it,
                     const std::string::iterator& end, table*& curr_table)
    {
        // remove the beginning keytable marker
        ++it;
        if (it == end)
            throw_parse_exception("Unexpected end of table");
        if (*it == '[')
            parse_table_array(it, end, curr_table);
        else
            parse_single_table(it, end, curr_table);
    }

    void parse_single_table(std::string::iterator& it,
                            const std::string::iterator& end,
                            table*& curr_table)
    {
        if (it == end || *it == ']')
            throw_parse_exception("Table name cannot be empty");

        std::string full_table_name;
        bool inserted = false;
        while (it != end && *it != ']')
        {
            auto part = parse_key(it, end,
                                  [](char c) { return c == '.' || c == ']'; });

            if (part.empty())
                throw_parse_exception("Empty component of table name");

            if (!full_table_name.empty())
                full_table_name += ".";
            full_table_name += part;

            if (curr_table->contains(part))
            {
                auto b = curr_table->get(part);
                if (b->is_table())
                    curr_table = static_cast<table*>(b.get());
                else if (b->is_table_array())
                    curr_table = std::static_pointer_cast<table_array>(b)
                                     ->get()
                                     .back()
                                     .get();
                else
                    throw_parse_exception("Key " + full_table_name
                                          + "already exists as a value");
            }
            else
            {
                inserted = true;
                curr_table->insert(part, make_table());
                curr_table = static_cast<table*>(curr_table->get(part).get());
            }
            consume_whitespace(it, end);
            if (it != end && *it == '.')
                ++it;
            consume_whitespace(it, end);
        }

        // table already existed
        if (!inserted)
        {
            auto is_value
                = [](const std::pair<const std::string&,
                                     const std::shared_ptr<base>&>& p) {
                      return p.second->is_value();
                  };

            // if there are any values, we can't add values to this table
            // since it has already been defined. If there aren't any
            // values, then it was implicitly created by something like
            // [a.b]
            if (curr_table->empty() || std::any_of(curr_table->begin(),
                                                   curr_table->end(), is_value))
            {
                throw_parse_exception("Redefinition of table "
                                      + full_table_name);
            }
        }

        ++it;
        consume_whitespace(it, end);
        eol_or_comment(it, end);
    }

    void parse_table_array(std::string::iterator& it,
                           const std::string::iterator& end, table*& curr_table)
    {
        ++it;
        if (it == end || *it == ']')
            throw_parse_exception("Table array name cannot be empty");

        std::string full_ta_name;
        while (it != end && *it != ']')
        {
            auto part = parse_key(it, end,
                                  [](char c) { return c == '.' || c == ']'; });

            if (part.empty())
                throw_parse_exception("Empty component of table array name");

            if (!full_ta_name.empty())
                full_ta_name += ".";
            full_ta_name += part;

            consume_whitespace(it, end);
            if (it != end && *it == '.')
                ++it;
            consume_whitespace(it, end);

            if (curr_table->contains(part))
            {
                auto b = curr_table->get(part);

                // if this is the end of the table array name, add an
                // element to the table array that we just looked up
                if (it != end && *it == ']')
                {
                    if (!b->is_table_array())
                        throw_parse_exception("Key " + full_ta_name
                                              + " is not a table array");
                    auto v = b->as_table_array();
                    v->get().push_back(make_table());
                    curr_table = v->get().back().get();
                }
                // otherwise, just keep traversing down the key name
                else
                {
                    if (b->is_table())
                        curr_table = static_cast<table*>(b.get());
                    else if (b->is_table_array())
                        curr_table = std::static_pointer_cast<table_array>(b)
                                         ->get()
                                         .back()
                                         .get();
                    else
                        throw_parse_exception("Key " + full_ta_name
                                              + " already exists as a value");
                }
            }
            else
            {
                // if this is the end of the table array name, add a new
                // table array and a new table inside that array for us to
                // add keys to next
                if (it != end && *it == ']')
                {
                    curr_table->insert(part, make_table_array());
                    auto arr = std::static_pointer_cast<table_array>(
                        curr_table->get(part));
                    arr->get().push_back(make_table());
                    curr_table = arr->get().back().get();
                }
                // otherwise, create the implicitly defined table and move
                // down to it
                else
                {
                    curr_table->insert(part, make_table());
                    curr_table
                        = static_cast<table*>(curr_table->get(part).get());
                }
            }
        }

        // consume the last "]]"
        if (it == end)
            throw_parse_exception("Unterminated table array name");
        ++it;
        if (it == end)
            throw_parse_exception("Unterminated table array name");
        ++it;

        consume_whitespace(it, end);
        eol_or_comment(it, end);
    }

    void parse_key_value(std::string::iterator& it, std::string::iterator& end,
                         table* curr_table)
    {
        auto key = parse_key(it, end, [](char c) { return c == '='; });
        if (curr_table->contains(key))
            throw_parse_exception("Key " + key + " already present");
        if (*it != '=')
            throw_parse_exception("Value must follow after a '='");
        ++it;
        consume_whitespace(it, end);
        curr_table->insert(key, parse_value(it, end));
        consume_whitespace(it, end);
    }

    template <class Function>
    std::string parse_key(std::string::iterator& it,
                          const std::string::iterator& end, Function&& fun)
    {
        consume_whitespace(it, end);
        if (*it == '"')
        {
            return parse_quoted_key(it, end);
        }
        else
        {
            auto bke = std::find_if(it, end, std::forward<Function>(fun));
            return parse_bare_key(it, bke);
        }
    }

    std::string parse_bare_key(std::string::iterator& it,
                               const std::string::iterator& end)
    {
        if (it == end)
        {
            throw_parse_exception("Bare key missing name");
        }

        auto key_end = end;
        --key_end;
        consume_backwards_whitespace(key_end, it);
        ++key_end;
        std::string key{it, key_end};

        if (std::find(it, key_end, '#') != key_end)
        {
            throw_parse_exception("Bare key " + key + " cannot contain #");
        }

        if (std::find_if(it, key_end,
                         [](char c) { return c == ' ' || c == '\t'; })
            != key_end)
        {
            throw_parse_exception("Bare key " + key
                                  + " cannot contain whitespace");
        }

        if (std::find_if(it, key_end,
                         [](char c) { return c == '[' || c == ']'; })
            != key_end)
        {
            throw_parse_exception("Bare key " + key
                                  + " cannot contain '[' or ']'");
        }

        it = end;
        return key;
    }

    std::string parse_quoted_key(std::string::iterator& it,
                                 const std::string::iterator& end)
    {
        return string_literal(it, end, '"');
    }

    enum class parse_type
    {
        STRING = 1,
        DATE,
        INT,
        FLOAT,
        BOOL,
        ARRAY,
        INLINE_TABLE
    };

    std::shared_ptr<base> parse_value(std::string::iterator& it,
                                      std::string::iterator& end)
    {
        parse_type type = determine_value_type(it, end);
        switch (type)
        {
            case parse_type::STRING:
                return parse_string(it, end);
            case parse_type::DATE:
                return parse_date(it, end);
            case parse_type::INT:
            case parse_type::FLOAT:
                return parse_number(it, end);
            case parse_type::BOOL:
                return parse_bool(it, end);
            case parse_type::ARRAY:
                return parse_array(it, end);
            case parse_type::INLINE_TABLE:
                return parse_inline_table(it, end);
            default:
                throw_parse_exception("Failed to parse value");
        }
    }

    parse_type determine_value_type(const std::string::iterator& it,
                                    const std::string::iterator& end)
    {
        if (*it == '"' || *it == '\'')
        {
            return parse_type::STRING;
        }
        else if (is_date(it, end))
        {
            return parse_type::DATE;
        }
        else if (is_number(*it) || *it == '-' || *it == '+')
        {
            return determine_number_type(it, end);
        }
        else if (*it == 't' || *it == 'f')
        {
            return parse_type::BOOL;
        }
        else if (*it == '[')
        {
            return parse_type::ARRAY;
        }
        else if (*it == '{')
        {
            return parse_type::INLINE_TABLE;
        }
        throw_parse_exception("Failed to parse value type");
    }

    parse_type determine_number_type(const std::string::iterator& it,
                                     const std::string::iterator& end)
    {
        // determine if we are an integer or a float
        auto check_it = it;
        if (*check_it == '-' || *check_it == '+')
            ++check_it;
        while (check_it != end && is_number(*check_it))
            ++check_it;
        if (check_it != end && *check_it == '.')
        {
            ++check_it;
            while (check_it != end && is_number(*check_it))
                ++check_it;
            return parse_type::FLOAT;
        }
        else
        {
            return parse_type::INT;
        }
    }

    std::shared_ptr<value<std::string>> parse_string(std::string::iterator& it,
                                                     std::string::iterator& end)
    {
        auto delim = *it;
        assert(delim == '"' || delim == '\'');

        // end is non-const here because we have to be able to potentially
        // parse multiple lines in a string, not just one
        auto check_it = it;
        ++check_it;
        if (check_it != end && *check_it == delim)
        {
            ++check_it;
            if (check_it != end && *check_it == delim)
            {
                it = ++check_it;
                return parse_multiline_string(it, end, delim);
            }
        }
        return make_value<std::string>(string_literal(it, end, delim));
    }

    std::shared_ptr<value<std::string>>
    parse_multiline_string(std::string::iterator& it,
                           std::string::iterator& end, char delim)
    {
        std::stringstream ss;

        auto is_ws = [](char c) { return c == ' ' || c == '\t'; };

        bool consuming = false;
        std::shared_ptr<value<std::string>> ret;

        auto handle_line
            = [&](std::string::iterator& it, std::string::iterator& end) {
                  if (consuming)
                  {
                      it = std::find_if_not(it, end, is_ws);

                      // whole line is whitespace
                      if (it == end)
                          return;
                  }

                  consuming = false;

                  while (it != end)
                  {
                      auto check = it;
                      // handle escaped characters
                      if (delim == '"' && *it == '\\')
                      {
                          // check if this is an actual escape sequence or a
                          // whitespace escaping backslash
                          ++check;
                          if (check == end)
                          {
                              consuming = true;
                              break;
                          }

                          ss << parse_escape_code(it, end);
                          continue;
                      }

                      // if we can end the string
                      if (std::distance(it, end) >= 3)
                      {
                          auto check = it;
                          // check for """
                          if (*check++ == delim && *check++ == delim
                              && *check++ == delim)
                          {
                              it = check;
                              ret = make_value<std::string>(ss.str());
                              break;
                          }
                      }

                      ss << *it++;
                  }
              };

        // handle the remainder of the current line
        handle_line(it, end);
        if (ret)
            return ret;

        // start eating lines
        while (std::getline(input_, line_))
        {
            ++line_number_;

            it = line_.begin();
            end = line_.end();

            handle_line(it, end);

            if (ret)
                return ret;

            if (!consuming)
                ss << std::endl;
        }

        throw_parse_exception("Unterminated multi-line basic string");
    }

    std::string string_literal(std::string::iterator& it,
                               const std::string::iterator& end, char delim)
    {
        ++it;
        std::string val;
        while (it != end)
        {
            // handle escaped characters
            if (delim == '"' && *it == '\\')
            {
                val += parse_escape_code(it, end);
            }
            else if (*it == delim)
            {
                ++it;
                consume_whitespace(it, end);
                return val;
            }
            else
            {
                val += *it++;
            }
        }
        throw_parse_exception("Unterminated string literal");
    }

    char parse_escape_code(std::string::iterator& it,
                           const std::string::iterator& end)
    {
        ++it;
        if (it == end)
            throw_parse_exception("Invalid escape sequence");
        char value;
        if (*it == 'b')
        {
            value = '\b';
        }
        else if (*it == 't')
        {
            value = '\t';
        }
        else if (*it == 'n')
        {
            value = '\n';
        }
        else if (*it == 'f')
        {
            value = '\f';
        }
        else if (*it == 'r')
        {
            value = '\r';
        }
        else if (*it == '"')
        {
            value = '"';
        }
        else if (*it == '\\')
        {
            value = '\\';
        }
        else
        {
            throw_parse_exception("Invalid escape sequence");
        }
        ++it;
        return value;
    }

    std::shared_ptr<base> parse_number(std::string::iterator& it,
                                       const std::string::iterator& end)
    {
        // determine if we are an integer or a float
        auto check_it = it;

        auto eat_sign = [&]() {
            if (check_it != end && (*check_it == '-' || *check_it == '+'))
                ++check_it;
        };

        eat_sign();

        auto eat_numbers = [&]() {
            auto beg = check_it;
            while (check_it != end && is_number(*check_it))
            {
                ++check_it;
                if (check_it != end && *check_it == '_')
                {
                    ++check_it;
                    if (check_it == end || !is_number(*check_it))
                        throw_parse_exception("Malformed number");
                }
            }

            if (check_it == beg)
                throw_parse_exception("Malformed number");
        };

        eat_numbers();

        if (check_it != end
            && (*check_it == '.' || *check_it == 'e' || *check_it == 'E'))
        {
            bool is_exp = *check_it == 'e' || *check_it == 'E';

            ++check_it;
            if (check_it == end)
                throw_parse_exception("Floats must have trailing digits");

            if (is_exp)
                eat_sign();

            eat_numbers();

            if (!is_exp && check_it != end
                && (*check_it == 'e' || *check_it == 'E'))
            {
                ++check_it;
                eat_sign();
                eat_numbers();
            }

            return parse_float(it, check_it);
        }
        else
        {
            return parse_int(it, check_it);
        }
    }

    std::shared_ptr<value<int64_t>> parse_int(std::string::iterator& it,
                                              const std::string::iterator& end)
    {
        std::string v{it, end};
        v.erase(std::remove(v.begin(), v.end(), '_'), v.end());
        it = end;
        try
        {
            return make_value<int64_t>(std::stoll(v));
        }
        catch (const std::invalid_argument& ex)
        {
            throw_parse_exception("Malformed number (invalid argument: "
                                  + std::string{ex.what()} + ")");
        }
        catch (const std::out_of_range& ex)
        {
            throw_parse_exception("Malformed number (out of range: "
                                  + std::string{ex.what()} + ")");
        }
    }

    std::shared_ptr<value<double>> parse_float(std::string::iterator& it,
                                               const std::string::iterator& end)
    {
        std::string v{it, end};
        v.erase(std::remove(v.begin(), v.end(), '_'), v.end());
        it = end;
        try
        {
            return make_value<double>(std::stod(v));
        }
        catch (const std::invalid_argument& ex)
        {
            throw_parse_exception("Malformed number (invalid argument: "
                                  + std::string{ex.what()} + ")");
        }
        catch (const std::out_of_range& ex)
        {
            throw_parse_exception("Malformed number (out of range: "
                                  + std::string{ex.what()} + ")");
        }
    }

    std::shared_ptr<value<bool>> parse_bool(std::string::iterator& it,
                                            const std::string::iterator& end)
    {
        auto boolend = std::find_if(it, end, [](char c) {
            return c == ' ' || c == '\t' || c == '#' || c == ',' || c == ']';
        });
        std::string v{it, boolend};
        it = boolend;
        if (v == "true")
            return make_value<bool>(true);
        else if (v == "false")
            return make_value<bool>(false);
        else
            throw_parse_exception("Attempted to parse invalid boolean value");
    }

    std::string::iterator find_end_of_date(std::string::iterator it,
                                           std::string::iterator end)
    {
        return std::find_if(it, end, [this](char c) {
            return !is_number(c) && c != 'T' && c != 'Z' && c != ':' && c != '-'
                   && c != '+' && c != '.';
        });
    }

    std::shared_ptr<value<datetime>>
    parse_date(std::string::iterator& it, const std::string::iterator& end)
    {
        auto date_end = find_end_of_date(it, end);

        auto eat = [&](char c) {
            if (it == date_end || *it != c)
                throw_parse_exception("Malformed date");
            ++it;
        };

        auto eat_digits = [&](int len) {
            int val = 0;
            for (int i = 0; i < len; ++i)
            {
                if (!is_number(*it) || it == date_end)
                    throw_parse_exception("Malformed date");
                val = 10 * val + (*it++ - '0');
            }
            return val;
        };

        datetime dt;

        dt.year = eat_digits(4);
        eat('-');
        dt.month = eat_digits(2);
        eat('-');
        dt.day = eat_digits(2);
        eat('T');
        dt.hour = eat_digits(2);
        eat(':');
        dt.minute = eat_digits(2);
        eat(':');
        dt.second = eat_digits(2);

        if (*it == '.')
        {
            ++it;
            while (it != date_end && is_number(*it))
                dt.microsecond = 10 * dt.microsecond + (*it++ - '0');
        }

        if (it == date_end)
            throw_parse_exception("Malformed date");

        int hoff = 0;
        int moff = 0;
        if (*it == '+' || *it == '-')
        {
            auto plus = *it == '+';
            ++it;

            hoff = eat_digits(2);
            dt.hour_offset = (plus) ? hoff : -hoff;
            eat(':');
            moff = eat_digits(2);
            dt.minute_offset = (plus) ? moff : -moff;
        }
        else if (*it == 'Z')
        {
            ++it;
        }

        if (it != date_end)
            throw_parse_exception("Malformed date");

        return make_value(dt);
    }

    std::shared_ptr<base> parse_array(std::string::iterator& it,
                                      std::string::iterator& end)
    {
        // this gets ugly because of the "homogeneity" restriction:
        // arrays can either be of only one type, or contain arrays
        // (each of those arrays could be of different types, though)
        //
        // because of the latter portion, we don't really have a choice
        // but to represent them as arrays of base values...
        ++it;

        // ugh---have to read the first value to determine array type...
        skip_whitespace_and_comments(it, end);

        // edge case---empty array
        if (*it == ']')
        {
            ++it;
            return make_array();
        }

        auto val_end = std::find_if(
            it, end, [](char c) { return c == ',' || c == ']' || c == '#'; });
        parse_type type = determine_value_type(it, val_end);
        switch (type)
        {
            case parse_type::STRING:
                return parse_value_array<std::string>(it, end);
            case parse_type::INT:
                return parse_value_array<int64_t>(it, end);
            case parse_type::FLOAT:
                return parse_value_array<double>(it, end);
            case parse_type::DATE:
                return parse_value_array<datetime>(it, end);
            case parse_type::BOOL:
                return parse_value_array<bool>(it, end);
            case parse_type::ARRAY:
                return parse_object_array<array>(&parser::parse_array, '[', it,
                                                 end);
            case parse_type::INLINE_TABLE:
                return parse_object_array<table_array>(
                    &parser::parse_inline_table, '{', it, end);
            default:
                throw_parse_exception("Unable to parse array");
        }
    }

    template <class Value>
    std::shared_ptr<array> parse_value_array(std::string::iterator& it,
                                             std::string::iterator& end)
    {
        auto arr = make_array();
        while (it != end && *it != ']')
        {
            auto value = parse_value(it, end);
            if (auto v = value->as<Value>())
                arr->get().push_back(value);
            else
                throw_parse_exception("Arrays must be heterogeneous");
            skip_whitespace_and_comments(it, end);
            if (*it != ',')
                break;
            ++it;
            skip_whitespace_and_comments(it, end);
        }
        if (it != end)
            ++it;
        return arr;
    }

    template <class Object, class Function>
    std::shared_ptr<Object> parse_object_array(Function&& fun, char delim,
                                               std::string::iterator& it,
                                               std::string::iterator& end)
    {
        auto arr = make_element<Object>();

        while (it != end && *it != ']')
        {
            if (*it != delim)
                throw_parse_exception("Unexpected character in array");

            arr->get().push_back(((*this).*fun)(it, end));
            skip_whitespace_and_comments(it, end);

            if (*it != ',')
                break;

            ++it;
            skip_whitespace_and_comments(it, end);
        }

        if (it == end || *it != ']')
            throw_parse_exception("Unterminated array");

        ++it;
        return arr;
    }

    std::shared_ptr<table> parse_inline_table(std::string::iterator& it,
                                              std::string::iterator& end)
    {
        auto tbl = make_table();
        do
        {
            ++it;
            if (it == end)
                throw_parse_exception("Unterminated inline table");

            consume_whitespace(it, end);
            parse_key_value(it, end, tbl.get());
            consume_whitespace(it, end);
        } while (*it == ',');

        if (it == end || *it != '}')
            throw_parse_exception("Unterminated inline table");

        ++it;
        consume_whitespace(it, end);

        return tbl;
    }

    void skip_whitespace_and_comments(std::string::iterator& start,
                                      std::string::iterator& end)
    {
        consume_whitespace(start, end);
        while (start == end || *start == '#')
        {
            if (!std::getline(input_, line_))
                throw_parse_exception("Unclosed array");
            line_number_++;
            start = line_.begin();
            end = line_.end();
            consume_whitespace(start, end);
        }
    }

    void consume_whitespace(std::string::iterator& it,
                            const std::string::iterator& end)
    {
        while (it != end && (*it == ' ' || *it == '\t'))
            ++it;
    }

    void consume_backwards_whitespace(std::string::iterator& back,
                                      const std::string::iterator& front)
    {
        while (back != front && (*back == ' ' || *back == '\t'))
            --back;
    }

    void eol_or_comment(const std::string::iterator& it,
                        const std::string::iterator& end)
    {
        if (it != end && *it != '#')
            throw_parse_exception("Unidentified trailing character "
                                  + std::string{*it}
                                  + "---did you forget a '#'?");
    }

    bool is_number(char c)
    {
        return c >= '0' && c <= '9';
    }

    bool is_date(const std::string::iterator& it,
                 const std::string::iterator& end)
    {
        auto date_end = find_end_of_date(it, end);
        std::string to_match{it, date_end};
#if CPPTOML_HAS_STD_REGEX
        return std::regex_match(to_match, date_pattern_);
#else
        // this can be approximate; we just need a lookahead
        return to_match.length() >= 20 && to_match[4] == '-'
               && to_match[7] == '-' && to_match[10] == 'T'
               && to_match[13] == ':' && to_match[16] == ':';
#endif
    }

    std::istream& input_;
    std::string line_;
    std::size_t line_number_ = 0;
#if CPPTOML_HAS_STD_REGEX
    std::regex date_pattern_{
        "(\\d{4})-(\\d{2})-(\\d{2})T(\\d{2}):(\\d{2}):(\\d{"
        "2})(\\.\\d+)?(Z|(\\+|-)(\\d{2}):(\\d{2}))"};
#endif
};

/**
 * Utility function to parse a file as a TOML file. Returns the root table.
 * Throws a parse_exception if the file cannot be opened.
 */
inline std::shared_ptr<table> parse_file(const std::string& filename)
{
#if defined(BOOST_NOWIDE_FSTREAM_INCLUDED_HPP)
    boost::nowide::ifstream file{filename.c_str()};
#elif defined(NOWIDE_FSTREAM_INCLUDED_HPP)
    nowide::ifstream file{filename.c_str()};
#else
    std::ifstream file{filename};
#endif
    if (!file.is_open())
        throw parse_exception{filename + " could not be opened for parsing"};
    parser p{file};
    return p.parse();
}

/**
 * base implementation of accept() that calls visitor.visit() on the concrete
 * class.
 */
template <class Visitor, class... Args>
void base::accept(Visitor&& visitor, Args&&... args) const
{
    if (is_value())
    {
        if (auto v = as<std::string>())
        {
            visitor.visit(*v, std::forward<Args>(args)...);
        }
        else if (auto v = as<int64_t>())
        {
            visitor.visit(*v, std::forward<Args>(args)...);
        }
        else if (auto v = as<double>())
        {
            visitor.visit(*v, std::forward<Args>(args)...);
        }
        else if (auto v = as<cpptoml::datetime>())
        {
            visitor.visit(*v, std::forward<Args>(args)...);
        }
        else if (auto v = as<bool>())
        {
            visitor.visit(*v, std::forward<Args>(args)...);
        }
    }
    else if (is_table())
    {
        visitor.visit(static_cast<const table&>(*this),
                      std::forward<Args>(args)...);
    }
    else if (is_array())
    {
        visitor.visit(static_cast<const array&>(*this),
                      std::forward<Args>(args)...);
    }
    else if (is_table_array())
    {
        visitor.visit(static_cast<const table_array&>(*this),
                      std::forward<Args>(args)...);
    }
}

/**
 * Writer that can be passed to accept() functions of cpptoml objects and
 * will output valid TOML to a stream.
 */
class toml_writer
{
  public:
    /**
     * Construct a toml_writer that will write to the given stream
     */
    toml_writer(std::ostream& s) : stream_(s), has_naked_endline_(false)
    {
        // nothing
    }

  public:
    /**
     * Output a string element of the TOML tree
     */
    void visit(const value<std::string>& v, bool = false)
    {
        write(v);
    }

    /**
     * Output an integer element of the TOML tree
     */
    void visit(const value<int64_t>& v, bool = false)
    {
        write(v);
    }

    /**
     * Output a double element of the TOML tree
     */
    void visit(const value<double>& v, bool = false)
    {
        write(v);
    }

    /**
     * Output a datetime element of the TOML tree
     */
    void visit(const value<datetime>& v, bool = false)
    {
        write(v);
    }

    /**
     * Output a boolean element of the TOML tree
     */
    void visit(const value<bool>& v, bool = false)
    {
        write(v);
    }

    /**
     * Output a table element of the TOML tree
     */
    void visit(const table& t, bool in_array = false)
    {
        write_table_header(in_array);
        std::vector<std::string> values;
        std::vector<std::string> tables;

        for (const auto& i : t)
        {
            if (i.second->is_table() || i.second->is_table_array())
            {
                tables.push_back(i.first);
            }
            else
            {
                values.push_back(i.first);
            }
        }

        for (unsigned int i = 0; i < values.size(); ++i)
        {
            path_.push_back(values[i]);

            if (i > 0)
                endline();

            write_table_item_header(*t.get(values[i]));
            t.get(values[i])->accept(*this, false);
            path_.pop_back();
        }

        for (unsigned int i = 0; i < tables.size(); ++i)
        {
            path_.push_back(tables[i]);

            if (values.size() > 0 || i > 0)
                endline();

            write_table_item_header(*t.get(tables[i]));
            t.get(tables[i])->accept(*this, false);
            path_.pop_back();
        }

        endline();
    }

    /**
     * Output an array element of the TOML tree
     */
    void visit(const array& a, bool = false)
    {
        write("[");

        for (unsigned int i = 0; i < a.get().size(); ++i)
        {
            if (i > 0)
                write(", ");

            if (a.get()[i]->is_array())
            {
                a.get()[i]->as_array()->accept(*this, true);
            }
            else
            {
                a.get()[i]->accept(*this, true);
            }
        }

        write("]");
    }

    /**
     * Output a table_array element of the TOML tree
     */
    void visit(const table_array& t, bool = false)
    {
        for (unsigned int j = 0; j < t.get().size(); ++j)
        {
            if (j > 0)
                endline();

            t.get()[j]->accept(*this, true);
        }

        endline();
    }

  protected:
    /**
     * Write out a string.
     */
    void write(const value<std::string>& v)
    {
        write("\"");
        write(escape_string(v.get()));
        write("\"");
    }

    /**
     * Write out an integer.
     */
    void write(const value<int64_t>& v)
    {
        write(v.get());
    }

    /**
     * Write out a double.
     */
    void write(const value<double>& v)
    {
        std::ios::fmtflags flags{stream_.flags()};

        stream_ << std::showpoint;
        write(v.get());

        stream_.flags(flags);
    }

    /**
     * Write out a datetime.
     */
    void write(const value<datetime>& v)
    {
        write(v.get());
    }

    /**
     * Write out a boolean.
     */
    void write(const value<bool>& v)
    {
        write((v.get() ? "true" : "false"));
    }

    /**
     * Write out the header of a table.
     */
    void write_table_header(bool in_array = false)
    {
        if (!path_.empty())
        {
            indent();

            write("[");

            if (in_array)
            {
                write("[");
            }

            for (unsigned int i = 0; i < path_.size(); ++i)
            {
                if (i > 0)
                {
                    write(".");
                }

                if (path_[i].find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcde"
                                               "fghijklmnopqrstuvwxyz0123456789"
                                               "_-")
                    == std::string::npos)
                {
                    write(path_[i]);
                }
                else
                {
                    write("\"");
                    write(escape_string(path_[i]));
                    write("\"");
                }
            }

            if (in_array)
            {
                write("]");
            }

            write("]");
            endline();
        }
    }

    /**
     * Write out the identifier for an item in a table.
     */
    void write_table_item_header(const base& b)
    {
        if (!b.is_table() && !b.is_table_array())
        {
            indent();

            if (path_.back().find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcde"
                                               "fghijklmnopqrstuvwxyz0123456789"
                                               "_-")
                == std::string::npos)
            {
                write(path_.back());
            }
            else
            {
                write("\"");
                write(escape_string(path_.back()));
                write("\"");
            }

            write(" = ");
        }
    }

  private:
    /**
     * Indent the proper number of tabs given the size of
     * the path.
     */
    void indent()
    {
        for (std::size_t i = 1; i < path_.size(); ++i)
            write("\t");
    }

    /**
     * Escape a string for output.
     */
    static std::string escape_string(const std::string& str)
    {
        std::string res;
        for (auto it = str.begin(); it != str.end(); ++it)
        {
            if (*it == '\\')
                res += "\\\\";
            else if (*it == '"')
                res += "\\\"";
            else if (*it == '\n')
                res += "\\n";
            else
                res += *it;
        }
        return res;
    }

    /**
     * Write a value out to the stream.
     */
    template <class T>
    void write(const T& v)
    {
        stream_ << v;
        has_naked_endline_ = false;
    }

    /**
     * Write an endline out to the stream
     */
    void endline()
    {
        if (!has_naked_endline_)
        {
            stream_ << "\n";
            has_naked_endline_ = true;
        }
    }

  private:
    std::ostream& stream_;
    std::vector<std::string> path_;
    bool has_naked_endline_;
};

inline std::ostream& operator<<(std::ostream& stream, const base& b)
{
    toml_writer writer{stream};
    b.accept(writer);
    return stream;
}

template <class T>
std::ostream& operator<<(std::ostream& stream, const value<T>& v)
{
    toml_writer writer{stream};
    v.accept(writer);
    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const table& t)
{
    toml_writer writer{stream};
    t.accept(writer);
    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const table_array& t)
{
    toml_writer writer{stream};
    t.accept(writer);
    return stream;
}

inline std::ostream& operator<<(std::ostream& stream, const array& a)
{
    toml_writer writer{stream};
    a.accept(writer);
    return stream;
}
}
#endif
