/*
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * This code is licensed under the terms of the GPLv2
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
 * MySQL Connectors. There are special exceptions to the terms and
 * conditions of the GPLv2 as it is applied to this software, see the
 * FLOSS License Exception
 * <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef MYSQL_CDK_PROTOCOL_MYSQLX_H
#define MYSQL_CDK_PROTOCOL_MYSQLX_H

/*

  An API for sending and receiving new mysqlx protocol messages
  =============================================================

  See doc/designs/protocol.rst for specifications.

*/

#include "../foundation.h"
#include "../foundation/opaque_impl.h"
#include "../api/query.h"
#include "mysqlx/traits.h"
#include "mysqlx/expr.h"


namespace cdk {
namespace protocol {
namespace mysqlx {

using cdk::foundation::byte;
using cdk::foundation::bytes;
using cdk::foundation::string;

/*
  Enumerations which define message type codes used by the protocol.

  Note: These enumerations are copied from protobuf generated headers.
  We do not want to introduce dependency of this public header on headers
  generated by protobuf and for that reason the constants must be updated
  manually whenever protobuf definitions change.

  TODO: Consider generating public header with message type
  constants. Such generated public header should be installed in
  a proper location.
*/

enum ClientMessages_Type {
  ClientMessages_Type_CON_CAPABILITIES_GET = 1,
  ClientMessages_Type_CON_CAPABILITIES_SET = 2,
  ClientMessages_Type_CON_CLOSE = 3,
  ClientMessages_Type_SESS_AUTHENTICATE_START = 4,
  ClientMessages_Type_SESS_AUTHENTICATE_CONTINUE = 5,
  ClientMessages_Type_SESS_RESET = 6,
  ClientMessages_Type_SESS_CLOSE = 7,
  ClientMessages_Type_SQL_STMT_EXECUTE = 12,
  ClientMessages_Type_CRUD_FIND = 17,
  ClientMessages_Type_CRUD_INSERT = 18,
  ClientMessages_Type_CRUD_UPDATE = 19,
  ClientMessages_Type_CRUD_DELETE = 20,
  ClientMessages_Type_EXPECT_OPEN = 24,
  ClientMessages_Type_EXPECT_CLOSE = 25,
  ClientMessages_Type_CRUD_CREATE_VIEW = 30,
  ClientMessages_Type_CRUD_MODIFY_VIEW = 31,
  ClientMessages_Type_CRUD_DROP_VIEW = 32
};

enum ServerMessages_Type {
  ServerMessages_Type_OK = 0,
  ServerMessages_Type_ERROR = 1,
  ServerMessages_Type_CONN_CAPABILITIES = 2,
  ServerMessages_Type_SESS_AUTHENTICATE_CONTINUE = 3,
  ServerMessages_Type_SESS_AUTHENTICATE_OK = 4,
  ServerMessages_Type_NOTICE = 11,
  ServerMessages_Type_RESULTSET_COLUMN_META_DATA = 12,
  ServerMessages_Type_RESULTSET_ROW = 13,
  ServerMessages_Type_RESULTSET_FETCH_DONE = 14,
  ServerMessages_Type_RESULTSET_FETCH_SUSPENDED = 15,
  ServerMessages_Type_RESULTSET_FETCH_DONE_MORE_RESULTSETS = 16,
  ServerMessages_Type_SQL_STMT_EXECUTE_OK = 17,
  ServerMessages_Type_RESULTSET_FETCH_DONE_MORE_OUT_PARAMS = 18
};


/*
  List of protocol messages.

  Each entry in the list defines one message, MSG_CLIENT() entry messages
  that are sent from client to server and MSG_SERVER() entry messages that
  server sends to client.

  Single entry MSG_CLIENT/SERVER(X, MSG, N, C) defines:

  :MSG: protobuf type of the message
  :N:   name of the msg_type enumeration constant for the message - for
        server-side messages the constant is msg_type::N and for
        client-side messages it is msg_type::cli_N.
  :C:   the value of msg_type enumeration constant - this is the message
        type code assigned by the protocol and defined using named constant
        from ClientMessages_Type or ServerMessages_Type enumeration.

  Parameter X is used to process the list in different ways (see definition
  of msg_type enumeration for an example).
*/


#define MSG_LIST(X) \
    MSG_CLIENT(X, Mysqlx::Connection::CapabilitiesGet, \
               CapabilitiesGet, CON_CAPABILITIES_GET) \
    MSG_CLIENT(X, Mysqlx::Connection::CapabilitiesSet, \
               CapabilitiesSet, CON_CAPABILITIES_SET) \
    MSG_CLIENT(X, Mysqlx::Connection::Close, \
               Close, CON_CLOSE) \
    MSG_CLIENT(X, Mysqlx::Session::AuthenticateStart, \
               AuthenticateStart, SESS_AUTHENTICATE_START) \
    MSG_CLIENT(X, Mysqlx::Session::AuthenticateContinue, \
               AuthenticateContinue, SESS_AUTHENTICATE_CONTINUE) \
    MSG_CLIENT(X, Mysqlx::Session::Reset, \
               SessionReset, SESS_RESET) \
    MSG_CLIENT(X, Mysqlx::Session::Close, \
               SessionClose, SESS_CLOSE) \
    MSG_CLIENT(X, Mysqlx::Sql::StmtExecute, \
               StmtExecute, SQL_STMT_EXECUTE) \
    MSG_CLIENT(X, Mysqlx::Crud::Find, \
               CrudFind, CRUD_FIND) \
    MSG_CLIENT(X, Mysqlx::Crud::Insert, \
               CrudInsert, CRUD_INSERT) \
    MSG_CLIENT(X, Mysqlx::Crud::Update, \
               CrudUpdate, CRUD_UPDATE) \
    MSG_CLIENT(X, Mysqlx::Crud::Delete, \
               CrudDelete, CRUD_DELETE) \
    MSG_CLIENT(X, Mysqlx::Crud::Delete, \
               ExpectOpen, EXPECT_OPEN) \
    MSG_CLIENT(X, Mysqlx::Crud::Delete, \
               ExpectClose, EXPECT_CLOSE) \
    MSG_CLIENT(X, Mysqlx::Crud::CreateView, CreateView, CRUD_CREATE_VIEW) \
    MSG_CLIENT(X, Mysqlx::Crud::ModifyView, ModifyView, CRUD_MODIFY_VIEW) \
    MSG_CLIENT(X, Mysqlx::Crud::DropView, DropView, CRUD_DROP_VIEW) \
\
    MSG_SERVER(X, Mysqlx::Ok, \
               Ok, OK) \
    MSG_SERVER(X, Mysqlx::Error, \
               Error, ERROR) \
    /* TODO: parameter changed (4) */ \
    MSG_SERVER(X, Mysqlx::Connection::Capabilities, \
               Capabilities, CONN_CAPABILITIES) \
    MSG_SERVER(X, Mysqlx::Session::AuthenticateContinue, \
               AuthenticateContinue, SESS_AUTHENTICATE_CONTINUE) \
    MSG_SERVER(X, Mysqlx::Session::AuthenticateOk, \
               AuthenticateOk, SESS_AUTHENTICATE_OK) \
    MSG_SERVER(X, Mysqlx::Notice::Frame, \
               Notice, NOTICE) \
    MSG_SERVER(X, Mysqlx::Resultset::ColumnMetaData, \
               ColumnMetaData, RESULTSET_COLUMN_META_DATA) \
    MSG_SERVER(X, Mysqlx::Resultset::Row, \
               Row, RESULTSET_ROW) \
    MSG_SERVER(X, Mysqlx::Resultset::FetchDone, \
               FetchDone, RESULTSET_FETCH_DONE) \
/*    MSG_SERVER(X, Mysqlx::Resultset::FetchSuspended, */ \
/*               SqlFetchSuspended, RESULTSET_FETCH_SUSPENDED)  */ \
    MSG_SERVER(X, Mysqlx::Resultset::FetchDoneMoreResultsets, \
               FetchDoneMoreResultsets, \
               RESULTSET_FETCH_DONE_MORE_RESULTSETS) \
    MSG_SERVER(X, Mysqlx::Resultset::FetchDoneMoreOutParams, \
               FetchDoneMoreOutParams, \
               RESULTSET_FETCH_DONE_MORE_OUT_PARAMS) \
    MSG_SERVER(X, Mysqlx::Sql::StmtExecuteOk, \
               StmtExecuteOk, SQL_STMT_EXECUTE_OK) \


#define MSG_CLIENT(X,MSG,N,C)  MSG_CLIENT_##X(MSG,N,C)
#define MSG_SERVER(X,MSG,N,C)  MSG_SERVER_##X(MSG,N,C)



/*
  Using MSG_LIST() define enumeration constant msg_type::N for
  each MSG_SERVER(X, MSG, N, C) entry in the list and msg_type::cli_N
  for each MSG_CLIENT(X, MSG, N, C) entry in the list. value of the
  constant is given by parameter C.
*/

#define MSG_CLIENT_enum(MSG,N,C) cli_##N= ClientMessages_Type_##C,
#define MSG_SERVER_enum(MSG,N,C) N= ServerMessages_Type_##C,

struct msg_type
{
  enum value {
    MSG_LIST(enum)
  };
};

typedef unsigned short int msg_type_t;

/*
  Column types used by X prtocol.

  Note: this must be in sync with Mysqlx::Sql::ColumnMetaData::FieldType
*/

#define COL_TYPE_LIST(X) \
    COL_TYPE(X, SINT     ,1) \
    COL_TYPE(X, UINT     ,2) \
    COL_TYPE(X, DOUBLE   ,5) \
    COL_TYPE(X, FLOAT    ,6) \
    COL_TYPE(X, BYTES    ,7) \
    COL_TYPE(X, TIME     ,10) \
    COL_TYPE(X, DATETIME ,12) \
    COL_TYPE(X, SET      ,15) \
    COL_TYPE(X, ENUM     ,16) \
    COL_TYPE(X, BIT      ,17) \
    COL_TYPE(X, DECIMAL  ,18) \

#define COL_TYPE(X,N,C) COL_TYPE_##X(N,C)

#define COL_TYPE_enum(N,C) N= C,

struct col_type
{
  enum value  {
    COL_TYPE_LIST(enum)
  };
};


/*
  Representation formats used for scalar values.

  Note: this must be in sync with Mysqlx::Datatypes::Scalar::Type enum.
*/


#define FMT_LIST(X) \
  VAL_FMT(X, SINT,    1) \
  VAL_FMT(X, UINT,    2) \
  VAL_FMT(X, VNULL,   3) \
  VAL_FMT(X, OCTETS,  4) \
  VAL_FMT(X, DOUBLE,  5) \
  VAL_FMT(X, FLOAT,   6) \
  VAL_FMT(X, BOOL,    7) \
  VAL_FMT(X, STRING,  8) \

#define VAL_FMT(X,N,C) VAL_FMT_##X(N,C)

#define VAL_FMT_enum(N,C) N= C,

struct val_fmt
{
  enum value {
    FMT_LIST(enum)
  };
};

/*
   Update Operation Type

   Must be in sync with Mysqlx::Crud::UpdateOperation::UpdateType
*/
#define UPDATE_OP_LIST(X) \
  VAL_UPDATE_OP(X, SET,         1) \
  VAL_UPDATE_OP(X, ITEM_REMOVE, 2) \
  VAL_UPDATE_OP(X, ITEM_SET,    3) \
  VAL_UPDATE_OP(X, ITEM_REPLACE,4) \
  VAL_UPDATE_OP(X, ITEM_MERGE,  5) \
  VAL_UPDATE_OP(X, ARRAY_INSERT,6) \
  VAL_UPDATE_OP(X, ARRAY_APPEND,7) \

#define VAL_UPDATE_OP(X,N,C) VAL_UPDATE_OP_##X(N,C)

#define VAL_UPDATE_OP_enum(N,C) N= C,

struct update_op
{
  enum value {
    UPDATE_OP_LIST(enum)
  };
};



#define NOTICE_TYPE_LIST(X) \
  NOTICE_TYPE(X,Warning, 1)               \
  NOTICE_TYPE(X,SessionVariableChanged,2) \
  NOTICE_TYPE(X,SessionStateChange,3)     \

#define NOTICE_TYPE(X,T,N) NOTICE_TYPE_##X(T,N)

#define NOTICE_TYPE_enum(T,N) T = N,

struct notice_type
{
  enum value {
    NOTICE_TYPE_LIST(enum)
  };
};


struct notice_scope
{
  enum value { GLOBAL = 1, LOCAL = 2 };
};



/*
  A class to store SQL state values.
*/

class sql_state_t
{
  char state[5];

public:

  sql_state_t()
  {
    memset(state, 0, sizeof(state));
  }

  sql_state_t(const std::string &s)
  {
    memcpy(state, s.c_str(), sizeof(state));
  }

  sql_state_t(const sql_state_t &other)
  {
    *this= other;
  }

  sql_state_t& operator=(const sql_state_t &other)
  {
    memcpy(state, other.state, sizeof(state));
    return *this;
  }

  bool operator==(const sql_state_t &other) const
  {
    return 0 == memcmp(state, other.state, sizeof(state));
  }

  operator std::string() const
  {
    return std::string(state, sizeof(state));
  }
};

inline
std::ostream& operator<<(std::ostream &out, const sql_state_t &sqlst)
{
  out <<(std::string)sqlst;
  return out;
}


class Auth_processor;
class Reply_processor;
class Result_processor;
class Stmt_processor;
class Row_processor;
class Mdata_processor;
class Row_source;
class Update_spec;

}}  // protocol::mysqlx


namespace protocol {
namespace mysqlx {


namespace api {

using foundation::string;

/*
  Public interface class for data exchange inside CDK representing
  Mysqlx::Crud::Collection message.
*/
class Db_obj
{
public:
  virtual const string& get_name() const = 0;
  virtual const string* get_schema() const = 0;
};


/*
  Public interface class for data exchange inside CDK representing
  Mysqlx::Crud::Limit message.
*/

typedef cdk::api::Doc_base<Scalar_processor> Args_map;
typedef cdk::api::Limit<row_count_t> Limit;
typedef cdk::api::Order_expr<Expression> Order_expr;
typedef cdk::api::Order_by<Expression> Order_by;
typedef cdk::api::Sort_direction Sort_direction;
typedef cdk::api::Projection<Expression> Projection;
typedef cdk::api::Columns Columns;

typedef cdk::api::View_options  View_options;

}  // api namespace


/*
  Interfaces which specify parameters for Find and other CRUD operations,
  such as Delete or Update, which can work on limited set of rwos/documents.

  Interface Select_spec specifies parameter common to all CRUD operations
  which limit set of rows/documents. Interface Find_spec specifies additional
  parameters of the Find operation.
*/

struct Select_spec
{
public:

  typedef api::Db_obj     Db_obj;
  typedef api::Expression Expression;
  typedef api::Order_by   Order_by;
  typedef api::Limit      Limit;

  virtual const Db_obj&     obj() const = 0;
  virtual const Expression* select() const = 0;
  virtual const Order_by*   order() const = 0;
  virtual const Limit*      limit() const = 0;
};

struct Find_spec : public Select_spec
{
public:

  typedef api::Projection  Projection;
  typedef api::Expr_list   Expr_list;

  virtual const Projection* project() const = 0;
  virtual const Expr_list*  group_by() const = 0;
  virtual const Expression* having() const = 0;
};


/*
  This type is used for optional parameters, so we need
  a DEFAULT type in addition to DOCUMENT and TABLE defined
  in the protocol specifications
*/
enum Data_model { DEFAULT= 0, DOCUMENT = 1, TABLE = 2 };

class Protocol
  : foundation::opaque_impl<Protocol>
  , foundation::nocopy
{
public:

  typedef cdk::api::Async_op<size_t> Op;

  class Stream;

  template <class C> Protocol(C &conn);

  Op& snd_CapabilitiesSet(const api::Any::Document& caps);
  Op& snd_AuthenticateStart(const char* mechanism, bytes data, bytes response);
  Op& snd_AuthenticateContinue(bytes data);
  Op& snd_Close();


  /**
    Send protocol command which executes a statement.

    Which statements are understood by the server are defined by the
    X Protocol. Currently we use "sql" namespace to execute plain SQL
    statements and commands in "admin" namespace for other operations
    and queries.

    @param ns   namespace used to interpret the statement
    @param stmt the statement to be eecuted
    @param args optional parameters of the statement
  */

  Op& snd_StmtExecute(const char *ns, const string &stmt,
                      const api::Any_list *args);


  /**
    Send CRUD Find command.

    This command returns rows from a table or documents from a collection.
    Selected data can be transformed using a projection before sending to
    the client.

    @param dm   determines whether this command fetches rows or documents

    @param spec  specifies source of the data, criteria selecting
      rows/documents to be returned, optional projection and other parameters
      of the commnad (@see Find_spec)

    @param args  if expressions used in the specification use named parameters,
      this argument map provides values of these parameters
  */

  Op& snd_Find(Data_model dm, const Find_spec &spec,
               const api::Args_map *args = NULL);

  /**
    Send CRUD Insert command.

    This command inserts rows into a table or documents into a collection

    @param dm   determines whether this command inserts rows or documents

    @param obj  target table or collection

    @param columns  optional specification determining which columns of the
      target table should be updated with provided data.

    @param data   Object defining rows/documents to be inserted. It is a
      sequence of tuples of expressions; each tuple in the sequence defines
      values of fields in a single row to be inserted (when inserting documents,
      there should be just one filed with the document).

    @param args  defines values of named parameters, if any are used in the
      expressions of the row source object
  */

  Op& snd_Insert(Data_model dm, api::Db_obj &obj,
                 const api::Columns *columns,
                 Row_source &data,
                 const api::Args_map *args = NULL);

  /**
    Send CRUD Update command.

    This command updates existing rows in a table or documents in a collection.
    In can work on a subset of rows or document defined by a select criteria.

    @param dm   determines whether this command updates rows or documents

    @param select  defines target table or collection whose data should be
      modified and a subset of rows/documents that is affected by the command

    @param update  a specification of what modifications should be applied to
      each row/document

    @param args  defines values of named parameters, if any are used in the
      selection criteria or update specification.
  */

  Op& snd_Update(Data_model dm,
                 const Select_spec &select,
                 Update_spec &update,
                 const api::Args_map *args = NULL);

  /**
    Send CRUD Delete command.

    This command removes all or selected rows/documents from a table or
    collection.

    @param dm   determines whether this command removes rows or documents

    @param select  defines target table or collection whose data should be
      modified and a subset of rows/documents to be deleted

    @param args  defines values of named parameters, if any are used in the
      selection criteria
  */

  Op& snd_Delete(Data_model dm, const Select_spec &select,
                 const api::Args_map *args = NULL);


  Op& snd_CreateView(Data_model dm, const api::Db_obj &obj,
                     const Find_spec &query, const api::Columns *columns,
                     api::View_options* = NULL,
                     const api::Args_map *args = NULL);
  Op& snd_ModifyView(Data_model dm, const api::Db_obj &obj,
                     const Find_spec &query, const api::Columns *columns,
                     api::View_options* = NULL,
                     const api::Args_map *args = NULL);
  Op& snd_DropView(const api::Db_obj &obj, bool if_exists);


  Op& rcv_AuthenticateReply(Auth_processor &);
  Op& rcv_Reply(Reply_processor &);
  Op& rcv_StmtReply(Stmt_processor &);
  Op& rcv_Rows(Row_processor &);
  Op& rcv_MetaData(Mdata_processor &);

private:

  class Impl;

  friend class  Impl;
  friend struct foundation::impl_traits<Protocol>;
};


typedef Protocol Protocol_client;


template <notice_type::value NT> struct Notice;

template <notice_type::value NT>
void process_notice(const bytes &, typename Notice<NT>::Processor&);


class Init_processor;
class Cmd_processor;


class Protocol_server
  : foundation::opaque_impl<Protocol_server>
  , foundation::nocopy
{

public:

  typedef Protocol::Op Op;

  template <class C> Protocol_server(C &conn);

  Op& snd_AuthenticateContinue(bytes data);
  Op& snd_AuthenticateOK(bytes data);
  Op& snd_Ok(const string &msg);
  Op& snd_Error(short unsigned errc, const string &msg);
  Op& snd_StmtExecuteOk();

  Op& rcv_InitMessage(Init_processor&);
  Op& rcv_Command(Cmd_processor&);

private:

  class Impl;

  friend class  Impl;
  friend struct foundation::impl_traits<Protocol_server>;
};


/*
  Wrapper class around different possible transport layer (connection)
  implementations. If C is a connection class then an object of class
  Protocol::Stream::Impl<C> implements methods read() and write() which
  create read or write operation, respectively, using appropriate operation
  type C::Read_op or C::Write_op. This operation is allocated dynamically
  and should be deleted by the caller of the method.
*/

class Protocol::Stream
{
public:

  typedef cdk::api::Stream_base::Op Op;

  Stream()
  {}
  virtual ~Stream()
  {}

  virtual Op* read(const buffers&) =0;
  virtual Op* write(const buffers&) =0;

private:

  /*
    This is not a general-use class. We only let Protocol implementation
    to access the Impl<> template and create instancs of Prtocol::Stream
    class.
  */

  template <class C> class Impl;
  friend class Protocol;
  friend class Protocol_server;
};


template <class C>
class Protocol::Stream::Impl : public Stream
{
  typedef typename C::Read_op  Rd_op;
  typedef typename C::Write_op Wr_op;

  C &m_conn;

  Impl(C &conn) : m_conn(conn) {}

  Op* read(const buffers &buf)
  { return new Rd_op(m_conn, buf); }

  Op* write(const buffers &buf)
  { return new Wr_op(m_conn, buf); }

  friend class Protocol;
  friend class Protocol_server;
};


/*
  Given connection class C and reference conn to a connection object
  of that class we create Protocol instance passing a Stream::Impl<C>
  wrapper around the connection to the internal implementation object.

  Internal implementation takes ownership of the connection wrapper
  and will delete it upon destruction.
*/

template <class C>
Protocol::Protocol(C &conn)
  : foundation::opaque_impl<Protocol>(NULL,
      static_cast<Protocol::Stream*>(new Protocol::Stream::Impl<C>(conn)))
{}


template <class C>
Protocol_server::Protocol_server(C &conn)
  : foundation::opaque_impl<Protocol_server>(NULL,
      static_cast<Protocol::Stream*>(new Protocol::Stream::Impl<C>(conn)))
{}


/*
  Callback interfaces to be implemented by message processor objects
  ==================================================================

  Processing a single received message starts with a call
  to :func:`message_begin` and ends by a call to :func:`message_end`. Between
  these two calls other processor callbacks are called which pass information
  extracted from the message, for example :class:`Row_processor` methods to
  inform about contents of a row in a result set.

  It is not specified how implementation handles raw message data that is
  received from server. It can store complete message in an internal buffer
  before processing message contents, or it can process message as it reads it.
  In either case, processor is informed about the fact that the whole message
  payload has been received via :func:`message_received` callback. A call
  to :func:`message_received` can happen before, during or after other calls to
  processor methods, depending on how reading of message payload is handled by
  receive operation. However, :func:`message_received` is always called
  after :func:`message_begin` and before :func:`message_end`.

  Processor can request that raw message payload is passed to it. The data is
  passed via one or more :func:`message_data` calls which happen
  after :func:`message_begin` and befroe :func:`message_received` calls.

  If inital :func:`message_begin` call succeeds then receive operation
  ensures that a matching call to :func:`message_end` will be made (so that
  processor can, e.g., free allocated resources inside :func:`message_end`).
  This is the case even if processing of message is interrupted for whatever
  reason, such as exception thrown in processor callback or error reported by
  server. However, if initial call to :func:`message_begin` throws exception
  then it means that processor could not prepare for processing this message
  and there is no corresponding :func:`message_end` call.

  In either case, if processor callback throws an exception this interrupts
  the receive operation which should cancel itself and report error informing
  about the fact that processing was interrupted because of error in the
  processor object. As noted above, if possible the operation should finish
  reading all expected messages from the server before reporting the error.

  If receive operation reads several messages from the server, message
  processor can stop this operation after any message. This is done by
  returning `false` from :func:`message_end` callback. If receive operation was
  stopped in this way, another receive operation must be created to resume
  processing of the pending messages from the server. Details depend on
  particular :func:`rcv_XXX` operation that is involved.

  .. note:: Processor callbacks can corrupt processing of incoming messages
    if they misbehave. They should not perform time consuming operations to
    ensure that internal logic used to process messages runs smoothly.
*/


class Processor_base
{
public:

  typedef protocol::mysqlx::byte        byte;
  typedef protocol::mysqlx::string      string;
  typedef protocol::mysqlx::msg_type_t  msg_type_t;

  /*
    Called when message header is received. The type of the message stored
    in the header is passed to the method. Argument ``flag`` informs if
    the message was expected (``true``) or not (``false``).

    On return from this method, processor can change value of the flag to
    change the way given message is processed:

      +----------+-----------------------------------------------+
      |          | flag value when calling :func:`message_begin` |
      |          +-----------------------+-----------------------+
      | on exit  | true                  | false                 |
      +==========+=======================+=======================+
      | true     | message was expected, | unexpected message,   |
      |          | parse it              | ignore it             |
      +----------+-----------------------+-----------------------+
      | false    | message was expected, | unexpected message,   |
      |          | do not parse          | do not ignore         |
      +----------+-----------------------+-----------------------+

    .. note:: In case of unexpected message (flag is false on input),
    if processor leaves flag false, the receive operation will stop with
    error. If processor requests ignoring the unexpected message, the
    receive operation will proceed with reading the next one.

    If :func:`message_begin` returns non-zero value then processor
    requests that message's raw data is sent to it via
    :func:`message_data` calls (see below). The value indicates maximum
    amount of data that should be sent in the first :func:`message_data`
    call.

    Processor can request raw message bytes regardless of whether
    message was expected or not and what is the exit value of the flag.
  */

  virtual size_t message_begin(msg_type_t type, bool &/*flag*/)
  {
    m_type= type;
    return 0;
  }

  /*
    This method is called if processor requested raw message data. It
    passes next fragment of the data which is never larger than the
    maximum allowed size returned by :func:`message_begin` or previous
    :func:`message_data` call. If this method returns zero then no more
    message data will be sent. End of message data is indicated by
    :func:`message_received` call.
  */

  virtual size_t message_data(bytes /*data*/) { return 0; }

  /*
    Called when whole message payload has been read. The number of bytes of
    the payload that have been read is passed as ``bytes_read``.
  */

  virtual void message_received(size_t /*bytes_read*/) {}

  /*
    This method is called at the end of processing given message, after all
    other processor callbacks. It can be used by processor to free any
    resources allocated to process the message. The value returned by this
    method indicates whether further messages (if any) should be read and
    processed by the receive operation. This is the case if method returns
    `true`. Otherwise, if method returns `false`, the receive operation is
    stopped at this point. To resume reading and processing remaining messages
    (if any) another receive operation must be created.

    A call to :func:`message_end` always matches the
    initial :func:`message_begin` call (if successful), even if processor
    requested skipping given message or processing of the message was
    interrupted for whatever reason. Only if initial :func:`message_begin` call
    has thrown error there is no corresponding :func:`message_end` call at the
    end.
  */

  virtual bool message_end() { return true; }

protected:

  size_t message_begin_internal(msg_type_t type, bool &flag);
  void message_received_internal(size_t bytes_read);

  msg_type_t m_type;
  size_t m_bytes_read;

  friend class Protocol_impl;
};


/*
  Client-side message processors
  ------------------------------
*/

class Error_processor : public Processor_base
{
public:

  typedef protocol::mysqlx::sql_state_t sql_state_t;

  virtual void error(unsigned int /*code*/, short int /*severity*/,
                     sql_state_t /*sql_state*/, const string &/*msg*/)
  {}

  virtual void notice(unsigned int /*type*/,
                      short int /*scope*/,
                      bytes /*payload*/)
  {}
};


class Auth_processor : public Error_processor
{
public:

  virtual void auth_ok(bytes data) = 0;
  virtual void auth_continue(bytes data) = 0;
};


class Reply_processor : public Error_processor
{
public:
  virtual void ok(string /*msg*/) {}
};

class Stmt_processor : public Error_processor
{
public:
  virtual void prepare_ok() {}
  virtual void execute_ok() {}
  virtual void cursor_close_ok() {}
  virtual void stmt_close_ok() {}
};

class Row_processor : public Error_processor
{
public:

  typedef protocol::mysqlx::row_count_t row_count_t;
  typedef protocol::mysqlx::col_count_t col_count_t;

  virtual bool row_begin(row_count_t /*row*/) { return true; }
  virtual void row_end(row_count_t /*row*/) {}

  virtual void col_null(col_count_t /*pos*/) {}

  virtual size_t col_begin(col_count_t /*pos*/, size_t /*data_len*/) { return 0; }
  virtual size_t col_data(col_count_t /*pos*/, bytes /*data*/) { return 0;}
  virtual void col_end(col_count_t /*pos*/, size_t /*data_len*/) {}

  /*
    Note:
    The first flag informs if all rows from the result set have been processed.
    This can be not the case if only limited number of rows has been requested.

    The second flag informs if there is anothr result set following the first one.
  */

  virtual void done(bool /*eod*/, bool /*more*/) {}
};

class Mdata_processor : public Reply_processor
{
public:

  typedef protocol::mysqlx::col_count_t     col_count_t;
  typedef protocol::mysqlx::collation_id_t  collation_id_t;

  virtual void col_count(col_count_t) {}
  virtual void col_type(col_count_t /*pos*/, unsigned short /*type*/) {}
  virtual void col_name(col_count_t /*pos*/,
                        const string &/*name*/, const string &/*original*/) {}
  virtual void col_table(col_count_t /*pos*/,
                         const string &/*table*/, const string &/*original*/) {}
  virtual void col_schema(col_count_t /*pos*/,
                          const string &/*schema*/, const string &/*catalog*/) {}
  virtual void col_collation(col_count_t /*pos*/, collation_id_t /*cs*/) {}
  virtual void col_length(col_count_t /*pos*/, uint32_t /*length*/) {}
  virtual void col_decimals(col_count_t /*pos*/, unsigned short /*decimals*/) {}
  virtual void col_content_type(col_count_t /*pos*/, unsigned short /*type*/) {}
  virtual void col_flags(col_count_t /*pos*/, uint32_t /*flags*/) {}

  size_t message_begin(msg_type_t type, bool &flag)
  {
    size_t howmuch = Error_processor::message_begin(type, flag);

    // Ignore messages which terminate meta-data

    switch (type)
    {
    case msg_type::FetchDone:
    case msg_type::FetchDoneMoreResultsets:
    case msg_type::FetchDoneMoreOutParams:
      flag = false;
    default:
      break;
    }

    return howmuch;
  }
};


class Result_processor
  : public Mdata_processor
  , public Row_processor
{
public:
  virtual void execute_ok() {}
};


class SessionState_processor
{
public:

  typedef protocol::mysqlx::row_count_t row_count_t;
  typedef protocol::mysqlx::insert_id_t insert_id_t;

  enum row_stats_t { ROWS_AFFECTED, ROWS_FOUND, ROWS_MATCHED };
  enum trx_event_t { COMMIT, ROLLBACK };

  virtual void client_id(unsigned long) {}
  virtual void account_expired() {}
  virtual void current_schema(const string&) {}
  virtual void row_stats(row_stats_t, row_count_t) {}
  virtual void last_insert_id(insert_id_t) {}
  virtual void trx_event(trx_event_t) {}
};

template<>
struct Notice<notice_type::SessionStateChange>
{
  typedef SessionState_processor Processor;
};

template<>
struct Notice<notice_type::Warning>
{
  typedef Error_processor Processor;
};


class Row_source
  : public foundation::Iterator
  , public api::Expr_list
{
public:
  typedef api::Expression       Expression;
};


// TODO: move it to api namespace (together with other interfaces)

class Update_processor
{
public:

  typedef protocol::mysqlx::string        string;
  typedef protocol::mysqlx::api::Db_obj   Db_obj;
  typedef protocol::mysqlx::api::Doc_path Doc_path;
  typedef protocol::mysqlx::api::Expression::Processor Expr_prc;

  virtual void target_name(const string&) = 0;
  virtual void target_table(const api::Db_obj&) = 0;
  virtual void target_path(const api::Doc_path&) = 0;
  virtual Expr_prc* update_op(update_op::value ) = 0;

  virtual ~Update_processor() {}
};

class Update_spec
  : public cdk::api::Expr_base<Update_processor>
  , public foundation::Iterator
{
};


/*
  Server-side message processors
  ------------------------------
*/

class Init_processor : public Processor_base
{
public:
  virtual void auth_start(const char *mech, bytes data, bytes response) = 0;
  virtual void auth_continue(bytes data) = 0;
};


class Row_source_server
{
public:
  virtual bool col_begin(col_count_t pos, int &fmt) = 0;
  virtual bytes col_data(col_count_t pos) = 0;
};


class Cmd_processor : public Processor_base
{
public:
  virtual void close() {}
};

/*
  Simple class implementing api::Db_obj interface for using in
  situations when any special data processing is not required.
*/
class Db_obj : public api::Db_obj
{

private:
  string m_name, m_schema;
  bool m_schema_set;
public:

  Db_obj(string name) : m_name(name), m_schema_set(false) {}

  Db_obj(string name, string schema) : m_name(name), m_schema(schema),
                                                m_schema_set(true) {}

  Db_obj(const api::Db_obj &other)
    : m_name(other.get_name())
    , m_schema_set(false)
  {
    if (!other.get_schema())
      return;
    m_schema = *other.get_schema();
    m_schema_set = true;
  }

  virtual ~Db_obj() {}

  virtual const string& get_name() const { return m_name; };

  virtual const string* get_schema() const
  {
    return (m_schema_set ? &m_schema : NULL);
  };
};

class Order_by : public api::Order_by
{
  class Order_by_item
  {
    Expression &m_expr;
    api::Sort_direction m_sort_direction;
  public:

    Order_by_item(Expression &expr, api::Sort_direction sort_direction) :
                  m_expr(expr), m_sort_direction(sort_direction)
    {}

    const Expression& expr() { return m_expr; }
    api::Sort_direction direction() const { return m_sort_direction; }
  };

  std::vector<Order_by_item> m_items_list;

public:
  virtual uint32_t count() const
  {
    size_t cnt = m_items_list.size();
    assert(cnt <= std::numeric_limits<uint32_t>::max());
    return (uint32_t)cnt;
  }

  virtual const Expression& get(uint32_t pos)
  { return m_items_list[pos].expr(); }

  virtual api::Sort_direction get_direction(uint32_t pos) const
  {return m_items_list[pos].direction(); }
};


/*
  Simple class implementing api::Limit interface for using in
  situations when any special data processing is not required.
*/
class Limit : public api::Limit
{
private:
  const row_count_t m_row_count, m_offset;
  bool m_offset_set;
public:

  Limit() : m_row_count(0), m_offset(0), m_offset_set(false)
  {}

  Limit(row_count_t row_count) : m_row_count(row_count), m_offset(0), m_offset_set(false) {}

  Limit(row_count_t row_count, row_count_t offset) : m_row_count(row_count), m_offset(offset),
                                                m_offset_set(true) {}

  virtual row_count_t get_row_count() const { return m_row_count; };

  virtual const row_count_t* get_offset() const
  {
    if(m_offset_set)
      return &m_offset;
    return NULL;
  };
};


/*
  Simple class implementing api::Columns interface for using in
  situations when any special data processing is not required.
*/
class Columns : public api::Columns
{

  class Columns_data
  {
    private:
      string m_name, m_alias;
      bool m_name_set, m_alias_set;
    public:
      Columns_data(string *n, string *a) : m_name_set(false), m_alias_set(false)
      {
        if (n)
        {
          m_name = *n;
          m_name_set = true;
        }

        if (a)
        {
          m_alias = *a;
          m_alias_set = true;
        }
      }

      const string *get_name() const
      { return m_name_set ? &m_name : NULL; }

      const string *get_alias() const
      { return m_alias_set ? &m_alias : NULL; }
  };

  std::vector<Columns_data> m_col_list;

public:

  void add_columns(string *name, string *alias)
  {
    m_col_list.push_back(Columns_data(name, alias));
  }

  void process(Processor &prc) const
  {
    prc.list_begin();

    for (std::vector<Columns_data>::const_iterator it = m_col_list.begin();
         it != m_col_list.end();
         ++it)
    {
      const string *name = it->get_name();
      const string *alias = it->get_alias();
      if (!name)
        continue;
      safe_prc(prc)->list_el()->name(*name);
      if (alias)
        safe_prc(prc)->list_el()->alias(*alias);
    }

    prc.list_end();
  }

};

}}}


#endif
