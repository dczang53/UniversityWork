<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<!-- saved from url=(0062)http://web.cs.ucla.edu/classes/spring17/cs33/lab/pexexlab.html -->
<html><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8">

<title>Program Execution Explorer lab</title>
</head>

<body>

<h1>Program Execution Explorer lab</h1>

<h2>Useful pointers</h2>

<ul>
 <li><a href="https://sourceware.org/gdb/current/onlinedocs/gdb/">Debugging with GDB</a> (2017)</li>
 <li><a href="http://www.gnu.org/software/emacs/manual/html_node/elisp/Arithmetic-Operations.html">Arithmetic operations</a> in the GNU Elisp manual (2016) </li>
</ul>

<h2>Background</h2>

<p>The name of this assignment comes from the idea that a debugger
like GDB is better thought of as a way of exploring program execution
histories than merely as a debugger.</p>

<p>Although the GNU Emacs text editor is not intended for high
  performance numeric computation, its scripting
  language <a href="https://www.gnu.org/software/emacs/manual/html_node/elisp/index.html">Elisp</a>
  is reasonably widely used and Elisp applications need adequate (if
  not stellar) performance for numeric applications.  One such
  application, <a href="http://www.gnu.org/software/emacs/manual/html_node/calc/index.html">GNU
  Calc</a>, is a desk calculator that does a lot of arithmetic
  internally. Your goal is to see how much overhead is imposed by
  Emacs when doing standard arithmetic operations, in particular
  multiplication with integer arguments, and to think about how
  to reduce the arithmetic overhead.</p>

<h2>Keep a log</h2>

<p>Keep a log in the file <samp>pexexlab.txt</samp> of what you do in
the lab so that you can reproduce the results later. This should not
merely be a transcript of what you typed: it should be more like a lab
notebook, in which you briefly note down what you did and what
happened. It should record not just what worked, but also
what <em>didn't</em> work.</p>

<h2>Gather instruction traces</h2>

<p>You can multiply numbers with Emacs from the shell by running a
  command like this:

  </p><pre><samp>  emacs -Q -batch -eval '(print (* 1250284240 -1844255039))'</samp></pre>

<p>Gather a trace for the key part of the above test case.  This trace
should include every instruction in the <samp>Ftimes</samp> function,
which is the C implementation of the Elisp <samp>*</samp> function.
It should also include every instruction in every function
  that <samp>Ftimes</samp> calls, either directly or indirectly.</p>

<p>For the purpose of this assignment, a trace is an ASCII text file.
  Each line corresponds to a single machine instruction executed in turn
  during the trace. Lines use the following format:</p>

  <pre><samp>5477fd	41 57	push %r15	M8[7fffffffd380]=0 rsp=7fffffffd380</samp></pre>

<p>Columns should be separated by
  single <a href="https://en.wikipedia.org/wiki/Tab_character">tab
  characters</a>.  The first
  column gives the machine address of the instruction, in hexadecimal
  (without leading <samp>0x</samp> or excess leading <samp>0</samp>).
  The second column gives the bytes of the machine instruction,
  in hexadecimal (without leading <samp>0x</samp>) separated by single spaces.
  The third column gives the assembly-language
  equivalent of the second column, using the notation that GDB uses
  after you execute the command <a href="https://sourceware.org/gdb/onlinedocs/gdb/Machine-Code.html"><samp>set disassemble-next-line on</samp></a>, except
  with a single space separating each column.
  The fourth column gives the effect of the
  instruction on memory and general-purpose registers, again using
  unsigned hexadecimal in the same style as the first column.
  The example above stores 0 into the
  8-byte word at address 7fffffffd380 (because <samp>r15</samp> happens
  to be zero); the "8" in "M8" stands for
  an 8-byte memory access. The example also sets
  the <samp>rsp</samp> register to 7fffffffd380. List memory
  modifications in increasing address order, and register
  modifications in alphabetical order. Traces need not record
  modifications to status registers such as <samp>rflags</samp>.</p>

<p>To gather information for your trace
(which you should put into the file <samp>trace.tr</samp>),
use the executable <samp>~eggert/bin64/bin/emacs-25.2</samp> on
either <samp>lnxsrv07</samp> or <samp>lnxsrv09</samp>.
The corresponding source code
can be found in <samp>~eggert/src/emacs-25.2/</samp> (particularly
its <samp>src</samp> subdirctory), and the executable was compiled for
the x86-64. The above example trace line corresponds to 45 bytes
into the machine code for the function <samp>arith_driver</samp>,
which in turn corresponds to line 2635
of <samp>~eggert/src/emacs-25.2/src/data.c</samp>.</p>

<h2>Examine integer overflow handling</h2>

<p>Compile the following function:</p>
<pre><samp>  _Bool
  testovf (long n)
  {
    return n + 9223372036854775807 &lt; n;
  }</samp></pre>
<p>for the x86-64 in three ways: (1) with <samp>-O2</samp>, (2)
with <samp>-O2 -fwrapv</samp>, (3) with <samp>-O2
-fsanitize=undefined</samp>. Compare the resulting assembly-language
files, and describe and justify the differences that you see. Put your
description into a plain ASCII text file <samp>testovf.txt</samp>.</p>

<h2>A few more questions</h2>

<p>Answer the following questions, in a plain text file <samp>answers.txt</samp>:</p>

<ol>
<li> Explain why the instructions in the trace did not produce the correct mathematical result. Which instructions caused the problem, exactly?</li>

<li> Explain why the shell command <samp>emacs -Q -batch -eval '(print most-negative-fixnum)'</samp> outputs <samp>-2305843009213693952</samp>. Where did the number come from? Explain in terms of the Emacs source code.</li>

<li> Explain why the shell command <samp>emacs -Q -batch -eval '(print (* most-positive-fixnum most-positive-fixnum most-positive-fixnum most-positive-fixnum))'</samp> outputs only 1.</li>

<li> The Emacs executable was compiled with GCC's <samp>-O2</samp> option.
  Suppose it had also been compiled with <samp>-fwrapv</samp>.
  Explain any problems Emacs would run into, or if there would
  not be a problem explain why not.</li>

<li> There is a recently-discovered security vulnerability in Emacs 25.2, which
  you can exercise by running <samp>emacs -Q -batch -eval '(print
  (format-time-string "%Y-%m-%d %H:%M:%S %Z" nil (concat (make-string
    1000 ?X) "0")))'</samp>. Briefly describe the bug's low-level
  manifestation by crashing Emacs,
  using <a href="https://sourceware.org/gdb/onlinedocs/gdb/Backtrace.html">GDB's <samp>backtrace</samp>
  command</a>, and following up with any other GDB commands that you
  think might be of interest.</li>
</ol>

<h2>Submit</h2>

<p>Submit a compressed tarball <samp>pexex.tgz</samp> containing the
files mentioned above, namely
<samp>pexexlab.txt</samp>, <samp>trace.tr</samp>, <samp>testovf.txt</samp>,
and <samp>answers.txt</samp>.</p>

<hr>
<address>
 © 2015–2017 <a href="http://web.cs.ucla.edu/classes/spring17/cs33/mail-eggert.html">Paul Eggert</a>.
 See <a href="http://web.cs.ucla.edu/classes/spring17/cs33/copyright.html">copying rules</a>.<br>

 $Id: pexexlab.html,v 1.9 2017/04/28 04:49:44 eggert Exp $

</address>



</body></html>