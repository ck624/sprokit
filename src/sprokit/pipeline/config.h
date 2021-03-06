/*ckwg +29
 * Copyright 2011-2013 by Kitware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither name of Kitware, Inc. nor the names of any contributors may be used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SPROKIT_PIPELINE_CONFIG_H
#define SPROKIT_PIPELINE_CONFIG_H

#include "pipeline-config.h"

#include "types.h"
#include "utils.h"

#include <boost/optional/optional.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/noncopyable.hpp>

#include <map>
#include <set>
#include <string>
#include <sstream>
#include <vector>

#include <cstddef>

/**
 * \file config.h
 *
 * \brief Header for \link sprokit::config configuration\endlink in the pipeline.
 */

namespace sprokit
{

/**
 * \class config config.h <sprokit/pipeline/config.h>
 *
 * \brief Stores configuration values for use within a \ref pipeline.
 *
 * \ingroup base_classes
 */
class SPROKIT_PIPELINE_EXPORT config
  : public boost::enable_shared_from_this<config>
  , boost::noncopyable
{
  public:
    /// The type that represents a configuration value key.
    typedef std::string key_t;
    /// The type that represents a collection of configuration keys.
    typedef std::vector<key_t> keys_t;
    /// The type that represents a description of a configuration key.
    typedef std::string description_t;
    /// The type that represents a stored configuration value.
    typedef std::string value_t;

    /**
     * \brief Create an empty configuration.
     *
     * \param name The name of the configuration block.
     *
     * \returns An empty configuration block.
     */
    static config_t empty_config(key_t const& name = key_t());

    /**
     * \brief Destructor.
     */
    ~config();

    /**
     * \brief Get a subblock from the configuration.
     *
     * Retrieve an unlinked configuration subblock from the current
     * configuration. Changes made to it do not affect \c *this.
     *
     * \param key The name of the sub-configuration to retrieve.
     *
     * \returns A subblock with copies of the values.
     */
    config_t subblock(key_t const& key) const;

    /**
     * \brief Get a subblock view into the configuration.
     *
     * Retrieve a view into the current configuration. Changes made to \c *this
     * are seen through the view and vice versa.
     *
     * \param key The name of the sub-configuration to retrieve.
     *
     * \returns A subblock which links to the \c *this.
     */
    config_t subblock_view(key_t const& key);

    /**
     * \brief Internally cast the value.
     *
     * \throws no_such_configuration_value_exception Thrown if the requested index does not exist.
     * \throws bad_configuration_cast_exception Thrown if the cast fails.
     *
     * \param key The index of the configuration value to retrieve.
     *
     * \returns The value stored within the configuration.
     */
    template <typename T>
    T get_value(key_t const& key) const;

    /**
     * \brief Cast the value, returning a default value in case of an error.
     *
     * \param key The index of the configuration value to retrieve.
     * \param def The value \p key does not exist or the cast fails.
     *
     * \returns The value stored within the configuration, or \p def if something goes wrong.
     */
    template <typename T>
    T get_value(key_t const& key, T const& def) const SPROKIT_NOTHROW;

    /**
     * \brief Set a value within the configuration.
     *
     * \throws set_on_read_only_value_exception Thrown if \p key is marked as read-only.
     *
     * \postconds
     *
     * \postcond{<code>this->get_value<value_t>(key) == value</code>}
     *
     * \endpostconds
     *
     * \param key The index of the configuration value to set.
     * \param value The value to set for the \p key.
     */
    void set_value(key_t const& key, value_t const& value);

    /**
     * \brief Remove a value from the configuration.
     *
     * \throws unset_on_read_only_value_exception Thrown if \p key is marked as read-only.
     * \throws no_such_configuration_value_exception Thrown if the requested index does not exist.
     *
     * \postconds
     *
     * \postcond{<code>this->get_value<T>(key)</code> throws \c no_such_configuration_value_exception}
     *
     * \endpostconds
     *
     * \param key The index of the configuration value to unset.
     */
    void unset_value(key_t const& key);

    /**
     * \brief Query if a value is read-only.
     *
     * \param key The key of the value query.
     *
     * \returns True if \p key is read-only, false otherwise.
     */
    bool is_read_only(key_t const& key) const;

    /**
     * \brief Set the value within the configuration as read-only.
     *
     * \postconds
     *
     * \postcond{<code>this->is_read_only(key) == true</code>}
     *
     * \endpostconds
     *
     * \param key The key of the value to mark as read-only.
     */
    void mark_read_only(key_t const& key);

    /**
     * \brief Merge the values in \p config into the current config.
     *
     * \note Any values currently set within \c *this will be overwritten if conficts occur.
     *
     * \throws set_on_read_only_value_exception Thrown if \p key is marked as read-only.
     *
     * \postconds
     *
     * \postcond{\c this->available_values() ⊆ \c config->available_values()}
     *
     * \endpostconds
     *
     * \param config The other configuration.
     */
    void merge_config(config_t const& config);

    /**
     * \brief Return the values available in the configuration.
     *
     * \returns All of the keys available within the block.
     */
    keys_t available_values() const;

    /**
     * \brief Check if a value exists for \p key.
     *
     * \param key The index of the configuration value to check.
     *
     * \returns Whether the key exists.
     */
    bool has_value(key_t const& key) const;

    /**
     * \brief Format configuration block to stream.
     *
     * \param str Config is formatted to this stream.
     */
    void print(std::ostream& str);

    /// The separator between blocks.
    static key_t const block_sep;
    /// The magic group for global parameters.
    static key_t const global_value;

  private:
    SPROKIT_PIPELINE_NO_EXPORT config(key_t const& name, config_t parent);

    boost::optional<value_t> find_value(key_t const& key) const;
    SPROKIT_PIPELINE_NO_EXPORT value_t get_value(key_t const& key) const;

    typedef std::map<key_t, value_t> store_t;
    typedef std::set<key_t> ro_list_t;

    config_t m_parent;
    key_t m_name;
    store_t m_store;
    ro_list_t m_ro_list;
};

/**
 * \class configuration_exception config.h <sprokit/pipeline/config.h>
 *
 * \brief The base class for all exceptions thrown from \ref config.
 *
 * \ingroup exceptions
 */
class SPROKIT_PIPELINE_EXPORT configuration_exception
  : public pipeline_exception
{
  public:
    /**
     * \brief Constructor.
     */
    configuration_exception() SPROKIT_NOTHROW;
    /**
     * \brief Destructor.
     */
    virtual ~configuration_exception() SPROKIT_NOTHROW;
};

/**
 * \class bad_configuration_cast config.h <sprokit/pipeline/config.h>
 *
 * \brief The inner exception thrown when casting fails.
 *
 * \ingroup exceptions
 */
class SPROKIT_PIPELINE_EXPORT bad_configuration_cast
  : public configuration_exception
{
  public:
    /**
     * \brief Constructor.
     *
     * \param reason The reason for the bad cast.
     */
    bad_configuration_cast(char const* reason) SPROKIT_NOTHROW;
    /**
     * \brief Destructor.
     */
    ~bad_configuration_cast() SPROKIT_NOTHROW;
};

/**
 * \class no_such_configuration_value_exception config.h <sprokit/pipeline/config.h>
 *
 * \brief Thrown when a value is requested for a value which does not exist.
 *
 * \ingroup exceptions
 */
class SPROKIT_PIPELINE_EXPORT no_such_configuration_value_exception
  : public configuration_exception
{
  public:
    /**
     * \brief Constructor.
     *
     * \param key The key that was requested from the configuration.
     */
    no_such_configuration_value_exception(config::key_t const& key) SPROKIT_NOTHROW;
    /**
     * \brief Destructor.
     */
    ~no_such_configuration_value_exception() SPROKIT_NOTHROW;

    /// The requested key name.
    config::key_t const m_key;
};

/**
 * \class bad_configuration_cast_exception config.h <sprokit/pipeline/config.h>
 *
 * \brief Thrown when a value cannot be converted to the requested type.
 *
 * \ingroup exceptions
 */
class SPROKIT_PIPELINE_EXPORT bad_configuration_cast_exception
  : public configuration_exception
{
  public:
    /**
     * \brief Constructor.
     *
     * \param key The key that was requested.
     * \param value The value that was failed to cast.
     * \param type The type that was requested.
     * \param reason The reason for the bad cast.
     */
    bad_configuration_cast_exception(config::key_t const& key, config::value_t const& value, char const* type, char const* reason) SPROKIT_NOTHROW;
    /**
     * \brief Destructor.
     */
    ~bad_configuration_cast_exception() SPROKIT_NOTHROW;

    /// The requested key name.
    config::key_t const m_key;
    /// The value of the requested key.
    config::value_t const m_value;
    /// The type requested for the cast.
    std::string const m_type;
    /// The reason for the failed cast.
    std::string const m_reason;
};

/**
 * \class set_on_read_only_value_exception config.h <sprokit/pipeline/config.h>
 *
 * \brief Thrown when a value is set but is marked as read-only.
 *
 * \ingroup exceptions
 */
class SPROKIT_PIPELINE_EXPORT set_on_read_only_value_exception
  : public configuration_exception
{
  public:
    /**
     * \brief Constructor.
     *
     * \param key The key that was requested from the configuration.
     * \param value The current read-only value of \p key.
     * \param new_value The value that was attempted to be set.
     */
    set_on_read_only_value_exception(config::key_t const& key, config::value_t const& value, config::value_t const& new_value) SPROKIT_NOTHROW;
    /**
     * \brief Destructor.
     */
    ~set_on_read_only_value_exception() SPROKIT_NOTHROW;

    /// The requested key name.
    config::key_t const m_key;
    /// The existing value.
    config::value_t const m_value;
    /// The new value.
    config::value_t const m_new_value;
};

/**
 * \class unset_on_read_only_value_exception config.h <sprokit/pipeline/config.h>
 *
 * \brief Thrown when a value is unset but is marked as read-only.
 *
 * \ingroup exceptions
 */
class SPROKIT_PIPELINE_EXPORT unset_on_read_only_value_exception
  : public configuration_exception
{
  public:
    /**
     * \brief Constructor.
     *
     * \param key The key that was requested from the configuration.
     * \param value The current value for \p key.
     */
    unset_on_read_only_value_exception(config::key_t const& key, config::value_t const& value) SPROKIT_NOTHROW;
    /**
     * \brief Destructor.
     */
    ~unset_on_read_only_value_exception() SPROKIT_NOTHROW;

    /// The requested key name.
    config::key_t const m_key;
    /// The existing value.
    config::value_t const m_value;
};

/**
 * \brief Default cast handling of configuration values.
 *
 * \note Do not use this in user code. Use \ref config_cast instead.
 *
 * \param value The value to convert.
 *
 * \throws bad_configuration_cast Thrown if lexical cast to
 * destination type fails.
 *
 * \returns The value of \p value in the requested type.
 */
template <typename T>
inline
T
config_cast_default(config::value_t const& value)
{
  try
  {
    return boost::lexical_cast<T>(value);
  }
  catch (boost::bad_lexical_cast const& e)
  {
    std::ostringstream ss;
    ss << e.what() << " - converting \"" << value << "\" to type \""
       << type_name<T>() << "\"";
    throw bad_configuration_cast( ss.str().c_str() );
  }
}

/**
 * \brief Type-specific casting handling.
 *
 * \note Do not use this in user code. Use \ref config_cast instead.
 *
 * \param value The value to convert.
 *
 * \returns The value of \p value in the requested type.
 */
template <typename T>
inline
T
config_cast_inner(config::value_t const& value)
{
  return config_cast_default<T>(value);
}

/**
 * \brief Type-specific casting handling.
 *
 * This is the \c bool specialization to handle \tt{true} and \tt{false}
 * literals versus just \tt{1} and \tt{0}.
 *
 * \note Do not use this in user code. Use \ref config_cast instead.
 *
 * \param value The value to convert.
 *
 * \returns The value of \p value in the requested type.
 */
template <>
SPROKIT_PIPELINE_EXPORT bool config_cast_inner(config::value_t const& value);

/**
 * \brief Cast a configuration value to the requested type.
 *
 * \throws bad_configuration_cast Thrown when the conversion fails.
 *
 * \param value The value to convert.
 *
 * \returns The value of \p value in the requested type.
 */
template <typename T>
inline
T
config_cast(config::value_t const& value)
{
  return config_cast_inner<T>(value);
}

template <typename T>
T
config
::get_value(key_t const& key) const
{
  boost::optional<value_t> value = find_value(key);

  if (!value)
  {
    throw no_such_configuration_value_exception(key);
  }

  try
  {
    return config_cast<T>(*value);
  }
  catch (bad_configuration_cast const& e)
  {
    throw bad_configuration_cast_exception(key, *value, typeid(T).name(), e.what());
  }
}

template <typename T>
T
config
::get_value(key_t const& key, T const& def) const SPROKIT_NOTHROW
{
  try
  {
    return get_value<T>(key);
  }
  catch (...)
  {
    return def;
  }
}

}

#endif // SPROKIT_PIPELINE_CONFIG_H
