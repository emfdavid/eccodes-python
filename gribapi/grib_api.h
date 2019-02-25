/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*! \file grib_api.h
  \brief grib_api C header file

*/

typedef enum ProductKind {PRODUCT_ANY, PRODUCT_GRIB, PRODUCT_BUFR, PRODUCT_METAR, PRODUCT_GTS, PRODUCT_TAF} ProductKind;

/*! Grib handle,   structure giving access to parsed message values by keys
    \ingroup grib_handle
*/
typedef struct grib_handle    grib_handle;

/*! Grib context,  structure containing the memory methods, the parsers and the formats.
    \ingroup grib_context
*/
typedef struct grib_context   grib_context;

/*! Grib keys iterator. Iterator over keys.
    \ingroup keys_iterator
*/
typedef struct grib_keys_iterator    grib_keys_iterator;

/*! \defgroup grib_index The grib_index
The grib_index is the structure giving indexed access to messages in a file.
 */
/*! @{*/

/*! index structure to access messages in a file.
*/
typedef struct grib_index grib_index;

/**
 *  Create a new index form a file. The file is indexed with the keys in argument.
 *
 * @param c           : context  (NULL for default context)
 * @param filename    : name of the file of messages to be indexed
 * @param keys        : comma separated list of keys for the index.
 *    The type of the key can be explicitly declared appending :l for long,
 *    (or alternatively :i)
 *    :d for double, :s for string to the key name. If the type is not
 *    declared explicitly, the native type is assumed.
 * @param err         :  0 if OK, integer value on error
 * @return            the newly created index
 */
grib_index* grib_index_new_from_file(grib_context* c,
                            char* filename,const char* keys,int *err);

/**
 *  Indexes the file given in argument in the index given in argument.
 *
 * @param index       : index
 * @param filename    : name of the file of messages to be indexed
 * @return            0 if OK, integer value on error
 */
int grib_index_write(grib_index *index, const char *filename);
grib_index* grib_index_read(grib_context* c,const char* filename,int *err);

/**
 *  Get the number of distinct values of the key in argument contained in the index. The key must belong to the index.
 *
 * @param index       : an index created from a file.
 *     The index must have been created with the key in argument.
 * @param key         : key for which the number of values is computed
 * @param size        : number of distinct values of the key in the index
 * @return            0 if OK, integer value on error
 */
int grib_index_get_size(grib_index* index,const char* key,size_t* size);

/**
 *  Get the distinct values of the key in argument contained in the index. The key must belong to the index. This function is used when the type of the key was explicitly defined as long or when the native type of the key is long.
 *
 * @param index       : an index created from a file.
 *     The index must have been created with the key in argument.
 * @param key         : key for which the values are returned
 * @param values      : array of values. The array must be allocated before entering this function and its size must be enough to contain all the values.
 * @param size        : size of the values array
 * @return            0 if OK, integer value on error
 */
int grib_index_get_long(grib_index* index,const char* key,
                        long* values,size_t *size);

/**
 *  Get the distinct values of the key in argument contained in the index. The key must belong to the index. This function is used when the type of the key was explicitly defined as string or when the native type of the key is string.
 *
 * @param index       : an index created from a file.
 *     The index must have been created with the key in argument.
 * @param key         : key for which the values are returned
 * @param values      : array of values. The array must be allocated before entering this function and its size must be enough to contain all the values.
 * @param size        : size of the values array
 * @return            0 if OK, integer value on error
 */
int grib_index_get_string(grib_index* index,const char* key,
                          char** values,size_t *size);

/**
 *  Select the message subset with key==value. The value is a string. The key must have been created with string type or have string as native type if the type was not explicitly defined in the index creation.
 *
 * @param index       : an index created from a file.
 *     The index must have been created with the key in argument.
 * @param key         : key to be selected
 * @param value       : value of the key to select
 * @return            0 if OK, integer value on error
 */
int grib_index_select_string(grib_index* index,const char* key,char* value);

/**
 *  Create a new handle from an index after having selected the key values.
 *  All the keys belonging to the index must be selected before calling this function. Successive calls to this function will return all the handles compatible with the constraints defined selecting the values of the index keys.
 * When no more handles are available from the index a NULL pointer is returned and the err variable is set to GRIB_END_OF_INDEX.
 *
 * @param index       : an index created from a file.
 * @param err         :  0 if OK, integer value on error. GRIB_END_OF_INDEX when no more handles are contained in the index.
 * @return            grib handle.
 */
grib_handle* grib_handle_new_from_index(grib_index* index,int *err);

/**
 *  Delete the index.
 *
 * @param index       : index to be deleted.
 */
void grib_index_delete(grib_index* index);

/*! @} */

/*! \defgroup grib_handle The grib_handle
The grib_handle is the structure giving access to parsed grib values by keys.
*/
/*! @{*/
/**
*  Counts the messages contained in a file resource.
*
* @param c           : the context from which the handle will be created (NULL for default context)
* @param f           : the file resource
* @param n           : the number of messages in the file
* @return            0 if OK, integer value on error
*/
int grib_count_in_file(grib_context* c, FILE* f,int* n);

/**
*  Create a handle from a file resource.
*  The file is read until a message is found. The message is then copied.
*  Remember always to delete the handle when it is not needed anymore to avoid
*  memory leaks.
*
* @param c           : the context from which the handle will be created (NULL for default context)
* @param f           : the file resource
* @param error       : error code set if the returned handle is NULL and the end of file is not reached
* @return            the new handle, NULL if the resource is invalid or a problem is encountered
*/
grib_handle* grib_handle_new_from_file(grib_context* c, FILE* f, int* error);

/**
*  Create a handle from a user message. The message is copied and will be freed with the handle
*
* @param c           : the context from which the handle will be created (NULL for default context)
* @param data        : the actual message
* @param data_len    : the length of the message in number of bytes
* @return            the new handle, NULL if the message is invalid or a problem is encountered
*/
grib_handle* grib_handle_new_from_message_copy(grib_context* c, const void* data, size_t data_len);


/**
 *  Create a handle from a GRIB message contained in the samples directory.
 *  The message is copied at the creation of the handle
 *
 * @param c           : the context from which the handle will be created (NULL for default context)
 * @param sample_name : the name of the sample file (without the .tmpl extension)
 * @return            the new handle, NULL if the resource is invalid or a problem is encountered
 */
grib_handle* grib_handle_new_from_samples (grib_context* c, const char* sample_name);



/**
*  Clone an existing handle using the context of the original handle,
*  The message is copied and reparsed
*
* @param h           : The handle to be cloned
* @return            the new handle, NULL if the message is invalid or a problem is encountered
*/
grib_handle* grib_handle_clone             (grib_handle* h)                 ;

/**
*  Frees a handle, also frees the message if it is not a user message
*  @see  grib_handle_new_from_message
* @param h           : The handle to be deleted
* @return            0 if OK, integer value on error
*/
int   grib_handle_delete   (grib_handle* h);

/*! @} */

/*! \defgroup handling_coded_messages Handling coded messages */
/*! @{ */
/**
* getting the message attached to a handle
*
* @param h              : the handle to which the buffer should be gathered
* @param message        : the pointer to be set to the handle's data
* @param message_length : On exit, the message size in number of bytes
* @return            0 if OK, integer value on error
*/
int grib_get_message(grib_handle* h ,const void** message, size_t *message_length);

/**
*  Get the number of coded value from a key, if several keys of the same name are present, the total sum is returned
*
* @param h           : the handle to get the offset from
* @param key         : the key to be searched
* @param size        : the address of a size_t where the size will be set
* @return            0 if OK, integer value on error
*/
int         grib_get_size(grib_handle* h, const char* key,size_t *size);

/**
*  Get the length of the string representation of the key, if several keys of the same name are present, the maximum length is returned
*
* @param h           : the handle to get the offset from
* @param key         : the key to be searched
* @param length        : the address of a size_t where the length will be set
* @return            0 if OK, integer value on error
*/
int         grib_get_length(grib_handle* h, const char* key,size_t *length);

/**
*  Get a long value from a key, if several keys of the same name are present, the last one is returned
*  @see  grib_set_long
*
* @param h           : the handle to get the data from
* @param key         : the key to be searched
* @param value       : the address of a long where the data will be retrieved
* @return            0 if OK, integer value on error
*/
int          grib_get_long         (grib_handle* h, const char* key, long*   value);

/**
*  Get a double value from a key, if several keys of the same name are present, the last one is returned
*  @see  grib_set_double
*
* @param h           : the handle to get the data from
* @param key         : the key to be searched
* @param value       : the address of a double where the data will be retrieved
* @return            0 if OK, integer value on error
*/
int grib_get_double       (grib_handle* h, const char* key, double* value);

/**
*  Get a string value from a key, if several keys of the same name are present, the last one is returned
* @see  grib_set_string
*
* @param h           : the handle to get the data from
* @param key         : the key to be searched
* @param mesg       : the address of a string where the data will be retrieved
* @param length      : the address of a size_t that contains allocated length of the string on input, and that contains the actual length of the string on output
* @return            0 if OK, integer value on error
*/
int grib_get_string(grib_handle* h, const char* key, char* mesg, size_t *length);

/**
*  Get string array values from a key. If several keys of the same name are present, the last one is returned
* @see  grib_set_string_array
*
* @param h       : the handle to get the data from
* @param key     : the key to be searched
* @param vals    : the address of a string array where the data will be retrieved
* @param length  : the address of a size_t that contains allocated length of the array on input, and that contains the actual length of the array on output
* @return        0 if OK, integer value on error
*/
int grib_get_string_array(grib_handle* h, const char* key, char** vals, size_t *length);

/**
*  Get double array values from a key. If several keys of the same name are present, the last one is returned
* @see  grib_set_double_array
*
* @param h           : the handle to get the data from
* @param key         : the key to be searched
* @param vals       : the address of a double array where the data will be retrieved
* @param length      : the address of a size_t that contains allocated length of the double array on input, and that contains the actual length of the double array on output
* @return            0 if OK, integer value on error
*/
int grib_get_double_array (grib_handle* h, const char* key, double* vals, size_t *length);

/**
*  Get long array values from a key. If several keys of the same name are present, the last one is returned
* @see  grib_set_long_array
*
* @param h           : the handle to get the data from
* @param key         : the key to be searched
* @param vals       : the address of a long array where the data will be retrieved
* @param length      : the address of a size_t that contains allocated length of the long array on input, and that contains the actual length of the long array on output
* @return            0 if OK, integer value on error
*/
int grib_get_long_array(grib_handle* h, const char* key, long* vals, size_t *length);

/**
*  Set a long value from a key. If several keys of the same name are present, the last one is set
*  @see  grib_get_long
*
* @param h           : the handle to set the data to
* @param key         : the key to be searched
* @param val         : a long where the data will be read
* @return            0 if OK, integer value on error
*/
int grib_set_long         (grib_handle* h, const char*  key , long val);

/**
*  Set a double value from a key. If several keys of the same name are present, the last one is set
*  @see  grib_get_double
*
* @param h           : the handle to set the data to
* @param key         : the key to be searched
* @param val       : a double where the data will be read
* @return            0 if OK, integer value on error
*/
int grib_set_double       (grib_handle* h, const char*  key , double   val);

/**
*  Set a string value from a key. If several keys of the same name are present, the last one is set
*  @see  grib_get_string
*
* @param h           : the handle to set the data to
* @param key         : the key to be searched
* @param mesg       : the address of a string where the data will be read
* @param length      : the address of a size_t that contains the length of the string on input, and that contains the actual packed length of the string on output
* @return            0 if OK, integer value on error
*/
int grib_set_string       (grib_handle* h, const char*  key , const char* mesg, size_t *length);

/**
*  Set a double array from a key. If several keys of the same name are present, the last one is set
*   @see  grib_get_double_array
*
* @param h           : the handle to set the data to
* @param key         : the key to be searched
* @param vals        : the address of a double array where the data will be read
* @param length      : a size_t that contains the length of the byte array on input
* @return            0 if OK, integer value on error
*/
int grib_set_double_array (grib_handle* h, const char*  key , const double*        vals   , size_t length);

/**
*  Turn on support for multiple fields in single grib messages
*
* @param c            : the context to be modified
*/
void grib_multi_support_on(grib_context* c);

/**
*  Turn off support for multiple fields in single GRIB messages
*
* @param c            : the context to be modified
*/
void grib_multi_support_off(grib_context* c);

/**
*  Get the API version
*
*  @return        API version
*/
long grib_get_api_version(void);

/*! \defgroup keys_iterator Iterating on keys names
The keys iterator is designed to get the key names defined in a message.
Key names on which the iteration is carried out can be filtered through their
attributes or by the namespace they belong to.
*/
/*! @{ */
/*! Create a new iterator from a valid and initialised handle.
*  @param h             : the handle whose keys you want to iterate
*  @param filter_flags  : flags to filter out some of the keys through their attributes
*  @param name_space    : if not null the iteration is carried out only on
*                         keys belonging to the namespace passed. (NULL for all the keys)
*  @return              keys iterator ready to iterate through keys according to filter_flags
*                       and namespace
*/
grib_keys_iterator* grib_keys_iterator_new(grib_handle* h,unsigned long filter_flags, const char* name_space);

/*! Step to the next iterator.
*  @param kiter         : valid grib_keys_iterator
*  @return              1 if next iterator exists, 0 if no more elements to iterate on
*/
int grib_keys_iterator_next(grib_keys_iterator* kiter);

/*! get the key name from the iterator
*  @param kiter         : valid grib_keys_iterator
*  @return              key name
*/
const char* grib_keys_iterator_get_name(grib_keys_iterator *kiter);

/*! Delete the iterator.
*  @param kiter         : valid grib_keys_iterator
*  @return              0 if OK, integer value on error
*/
int grib_keys_iterator_delete(grib_keys_iterator* kiter);

/**
* Convert an error code into a string
* @param code       : the error code
* @return           the error message
*/
const char* grib_get_error_message(int code);

int grib_get_native_type(grib_handle* h, const char* name,int* type);

/* aa: changed off_t to long int */
int grib_get_message_offset ( grib_handle* h,long int* offset);

int grib_is_missing(grib_handle* h, const char* key, int* err);
int grib_set_missing(grib_handle* h, const char* key);

/*! \defgroup errors Error codes
Error codes returned by the grib_api functions.
*/
/*! @{*/
/** No error */
#define GRIB_SUCCESS		0
/** End of resource reached */
#define GRIB_END_OF_FILE		-1
/** Internal error */
#define GRIB_INTERNAL_ERROR		-2
/** Passed buffer is too small */
#define GRIB_BUFFER_TOO_SMALL		-3
/** Function not yet implemented */
#define GRIB_NOT_IMPLEMENTED		-4
/** Missing 7777 at end of message */
#define GRIB_7777_NOT_FOUND		-5
/** Passed array is too small */
#define GRIB_ARRAY_TOO_SMALL		-6
/** File not found */
#define GRIB_FILE_NOT_FOUND		-7
/** Code not found in code table */
#define GRIB_CODE_NOT_FOUND_IN_TABLE		-8
/** Array size mismatch */
#define GRIB_WRONG_ARRAY_SIZE		-9
/** Key/value not found */
#define GRIB_NOT_FOUND		-10
/** Input output problem */
#define GRIB_IO_PROBLEM		-11
/** Message invalid */
#define GRIB_INVALID_MESSAGE		-12
/** Decoding invalid */
#define GRIB_DECODING_ERROR		-13
/** Encoding invalid */
#define GRIB_ENCODING_ERROR		-14
/** Code cannot unpack because of string too small */
#define GRIB_NO_MORE_IN_SET		-15
/** Problem with calculation of geographic attributes */
#define GRIB_GEOCALCULUS_PROBLEM		-16
/** Memory allocation error */
#define GRIB_OUT_OF_MEMORY		-17
/** Value is read only */
#define GRIB_READ_ONLY		-18
/** Invalid argument */
#define GRIB_INVALID_ARGUMENT		-19
/** Null handle */
#define GRIB_NULL_HANDLE		-20
/** Invalid section number */
#define GRIB_INVALID_SECTION_NUMBER		-21
/** Value cannot be missing */
#define GRIB_VALUE_CANNOT_BE_MISSING		-22
/** Wrong message length */
#define GRIB_WRONG_LENGTH		-23
/** Invalid key type */
#define GRIB_INVALID_TYPE		-24
/** Unable to set step */
#define GRIB_WRONG_STEP		-25
/** Wrong units for step (step must be integer) */
#define GRIB_WRONG_STEP_UNIT		-26
/** Invalid file id */
#define GRIB_INVALID_FILE		-27
/** Invalid grib id */
#define GRIB_INVALID_GRIB		-28
/** Invalid index id */
#define GRIB_INVALID_INDEX		-29
/** Invalid iterator id */
#define GRIB_INVALID_ITERATOR		-30
/** Invalid keys iterator id */
#define GRIB_INVALID_KEYS_ITERATOR		-31
/** Invalid nearest id */
#define GRIB_INVALID_NEAREST		-32
/** Invalid order by */
#define GRIB_INVALID_ORDERBY		-33
/** Missing a key from the fieldset */
#define GRIB_MISSING_KEY		-34
/** The point is out of the grid area */
#define GRIB_OUT_OF_AREA		-35
/** Concept no match */
#define GRIB_CONCEPT_NO_MATCH		-36
/** Hash array no match */
#define GRIB_HASH_ARRAY_NO_MATCH		-37
/** Definitions files not found */
#define GRIB_NO_DEFINITIONS		-38
/** Wrong type while packing */
#define GRIB_WRONG_TYPE		-39
/** End of resource */
#define GRIB_END		-40
/** Unable to code a field without values */
#define GRIB_NO_VALUES		-41
/** Grid description is wrong or inconsistent */
#define GRIB_WRONG_GRID		-42
/** End of index reached */
#define GRIB_END_OF_INDEX		-43
/** Null index */
#define GRIB_NULL_INDEX		-44
/** End of resource reached when reading message */
#define GRIB_PREMATURE_END_OF_FILE		-45
/** An internal array is too small */
#define GRIB_INTERNAL_ARRAY_TOO_SMALL		-46
/** Message is too large for the current architecture */
#define GRIB_MESSAGE_TOO_LARGE		-47
/** Constant field */
#define GRIB_CONSTANT_FIELD		-48
/** Switch unable to find a matching case */
#define GRIB_SWITCH_NO_MATCH		-49
/** Underflow */
#define GRIB_UNDERFLOW		-50
/** Message malformed */
#define GRIB_MESSAGE_MALFORMED		-51
/** Index is corrupted */
#define GRIB_CORRUPTED_INDEX		-52
/** Invalid number of bits per value */
#define GRIB_INVALID_BPV		-53
/** Edition of two messages is different */
#define GRIB_DIFFERENT_EDITION		-54
/** Value is different */
#define GRIB_VALUE_DIFFERENT		-55
/** Invalid key value */
#define GRIB_INVALID_KEY_VALUE		-56
/** String is smaller than requested */
#define GRIB_STRING_TOO_SMALL		-57
/** Wrong type conversion */
#define GRIB_WRONG_CONVERSION		-58
/** Missing BUFR table entry for descriptor */
#define GRIB_MISSING_BUFR_ENTRY		-59
/** Null pointer */
#define GRIB_NULL_POINTER		-60
/** Attribute is already present, cannot add */
#define GRIB_ATTRIBUTE_CLASH		-61
/** Too many attributes. Increase MAX_ACCESSOR_ATTRIBUTES */
#define GRIB_TOO_MANY_ATTRIBUTES		-62
/** Attribute not found. */
#define GRIB_ATTRIBUTE_NOT_FOUND		-63
/** Edition not supported. */
#define GRIB_UNSUPPORTED_EDITION		-64
/** Value out of coding range */
#define GRIB_OUT_OF_RANGE		-65
/** Size of bitmap is incorrect */
#define GRIB_WRONG_BITMAP_SIZE		-66
/*! @}*/