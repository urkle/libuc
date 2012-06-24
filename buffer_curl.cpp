/*
 * UniversalContainer library.
 * Copyright Jason Denton, 2008,2010.
 * Made available under the new BSD license, as described in LICENSE
 *
 * Send comments and bug reports to jason.denton@gmail.com
 * http://www.greatpanic.com/code.html
 */

/* Routines to use Buffers with curl. */

#include <curl/curl.h>
#include "buffer.h"

namespace JAD {

  //CURL receive call back
  size_t curl_receive_data(void* data, size_t size, size_t nmemb, void* b)
  {
    Buffer* buffer = static_cast<Buffer*>(b);
    char* tmp = (char*) data;
    size_t sent = size * nmemb;
    bool result = buffer->put_data(tmp,sent);
    if (result) return sent;
    else return 0;
  }
 
  //CURL send call back
  size_t curl_send_data(void* data, size_t size, size_t nmemb, void* b)
  {
    size_t want = size * nmemb; 
    Buffer* buffer = static_cast<Buffer*>(b);
    char* tmp = (char*) data; 
    return buffer->copy_out(tmp,want);
  }

  //Wrapper to setup a curl handle with appropriate send and
  //receive.
  CURL* setup_curl_handle(const char* url, Buffer* send, Buffer* receive)
  {
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
    curl_easy_setopt(curl,CURLOPT_NOSIGNAL,1);
    curl_easy_setopt(curl,CURLOPT_URL,url);

    if (send) {
      curl_easy_setopt(curl,CURLOPT_READFUNCTION, curl_send_data );
      curl_easy_setopt(curl,CURLOPT_READDATA, send);
    }

    if (receive) {
      curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, curl_receive_data);
      curl_easy_setopt(curl,CURLOPT_WRITEDATA, receive);
    }
    return curl;
  }

  //post a buffer, and get a buffer in response. Also get the mime-type
  //of the results.
  Buffer* http_post_buffer(const char* url, Buffer* send,
			   string& return_type, const char* send_type, const int timeout)
  {
    Buffer* receive = new Buffer;
    CURL* curl;
    char curl_error[CURL_ERROR_SIZE];
    size_t send_len = send->length - send->rpos;
    struct curl_slist* hlist=NULL;

    curl = setup_curl_handle(url,send,receive);
    curl_easy_setopt(curl,CURLOPT_ERRORBUFFER,curl_error);
    curl_easy_setopt(curl,CURLOPT_POST,1);
    curl_easy_setopt(curl,CURLOPT_POSTFIELDSIZE,send_len);   
    if (timeout > -1) curl_easy_setopt(curl,CURLOPT_TIMEOUT_MS,(long)timeout);
    
    if (!send_type) send_type = "application/x-www-form-urlencoded";
    hlist = curl_slist_append(NULL, send_type);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hlist);

    int rc = curl_easy_perform(curl);
    if (rc) {
      delete receive;
      return NULL;
    }
    
    char* rt = NULL;
    curl_easy_getinfo(curl,CURLINFO_CONTENT_TYPE,&rt);
    return_type = strtok(rt,";");
    curl_easy_cleanup(curl);
    return receive;
  }


  //get a web page into a buffer
  Buffer* http_get_buffer(const char* url, string& return_type, const int timeout)
  {
    Buffer* receive = new Buffer;
    CURL* curl;
    char curl_error[CURL_ERROR_SIZE];

    curl = setup_curl_handle(url,NULL,receive);
    curl_easy_setopt(curl,CURLOPT_ERRORBUFFER,curl_error);
    if (timeout > -1) curl_easy_setopt(curl,CURLOPT_TIMEOUT_MS,(long)timeout);

    int rc = curl_easy_perform(curl);
    if (rc) {
      delete receive;
      return NULL;
    }
    
    char* rt = NULL;
    curl_easy_getinfo(curl,CURLINFO_CONTENT_TYPE,&rt);
    return_type = strtok(rt,";");
    curl_easy_cleanup(curl);
    return receive;
  }


} //end namespace
