Lexical Analysis
===================

> Chapter Two, Modern Compiler Implementation in C.


comment handling
----------
 - Regex ___/*___ is used here to identify if we encounter the start of a comment string. When this happens, ___COMMENT_STATE___ begins.
 -  If we meet ___*/___ inside ___INITIAL_STATE___, we regard it as illegal since no comment is open.
 - Variable ___commentNesting___ is used to identify the depth of current nesting. 
 -  Inside ___COMMENT_STATE___, if the program meets another ___/*___, just consume it and increase ___commentNesting___. On the other hand, if it meets ___*/___, decrease ___commentNesting___. When ___commentNesting___ comes to zero, it means there are no more nested comments, and now go back to ___INITIAL_STATE___.
 - Some other situations are also handled inside  ___COMMENT_STATE___, like the ___EOF___ and ___/n___. Other inputs are considered legal and part of  the comment string.


string handling
--------------------
 - String handling is far more difficult than comment, since they are a lot of special cases to deal with.
 - We maintain a ___string_buffer___ to store the string content. The size of the buffer is set to 32 on default. When the string length comes up to the limit, double the buffer size. 
 - Specially, we also maintain a ___STRING_START___ to store the starting char pos. Every time we call ___adjust()___, the ___EM_tokPos___ increases. If we calculate the starting position using string length, sometimes it will come to incorrect results. 
 - Regex ___/"___ is used to identify the start of string literals. There are several situations inside ___STRING_STATE___.

|Raw string|How to handle|
| --- | --- |
|"|The end of string. Return  Special case is that we place an empty string will "(null)" to satisfy the test cases.|
|\n| Return is no t allowed in original string syntax.|
|\\\n, \\\t or ... |Push \n, \t, ... into the buffer.|
|___EOF___| Cause an error.|
|Other symbols| Considered legal and consume.|
	 

EOF handling
----------
 - As described above, ___EOF___ is gracefully handled in ___STRING_STATE___ and ___COMMENT_STATE___. 
 - The program will definitely end at an ___EOF___ sign. There is no need to handle that, since checking grammar is not the responsibility of lex.

error handling
----------------
 - As described above, regular errors like ___EOF___, trying to close a comment before open, illegal tokens are gracefully handled.

extra tests
--------
escape.tiger, to test regular escape characters like"\b", "\f", "\r". It is not included in this upload package, but performs correct in the console output.
