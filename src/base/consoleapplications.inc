{ applications.inc: A basic framework for representing a single process which
    uses a terminal window in an object-oriented fashion.

  This file is part of classwork.pp, a library of basic and useful classes.

  This program is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 3 of the License or, at your
  option, any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABLITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU General Public License along
  with this program.  If not, see <http://www.gnu.org/licenses/>.
}
{$IFDEF ReadInterface}
{ This class represents a single process running as a console application: a
  program that uses a terminal or console for standard input and output.  The
  class automatically creates three instances of @link(AStream): one each to
  represent @code(stdin), @code(stdout), and @code(stderr).

  You will likely not create an instance of @link(AConsoleApplication) directly,
  since it does not do much on its own; however, its methods are designed to
  easily add support for console I/O to a class which is derived from it.
}
type AConsoleApplication = class(AnApplication)
  protected
    { Refers to the stream used for standard output }
    MyConsoleOut: AStream;
    { Refers to the stream used for standard input }
    MyConsoleIn: AStream;
    { Refers to the stream used for standard error output }
    MyErrorOut: AStream;

  public
    { Initializer }
    function init: boolean; override;

    { Destroy the application.

      This method frees @link(AConsoleApplication.ConsoleOut),
      @link(AConsoleApplication.ConsoleIn), and
      @link(AConsoleApplication.ErrorOut) before calling the inherited
      routine.  It is called automatically when @code(TObject.free) is called
      on an instance of @link(AConsoleApplication).
    }
    destructor destroy; override;

    { Execute the application.

      This method overrides the behavior inherited from
      @link(AnApplication.run).  It constructs
      @link(AConsoleApplication.ConsoleOut) and
      @link(AConsoleApplication.ErrorOut) if they have not already been
      created.  This behavior allows descendant classes to construct their own
      stream to handle console output, input, and diagnostic output if desired.

      Note that @link(AConsoleApplication.ConsoleIn) is not automatically
      constructed in the base implementation of this method because not every
      application will need it.  Descendant classes may override this behavior.

      @return(Zero (0) if all three streams are valid (not @nil), either because
      a descendant class created them prior to the method being called, or
      because the method successfully created them itself.  If one or more of
      the streams are invalid (@nil), then this method will return a non-zero
      value.)
    }
    function run: integer; override;

    { Print the selected items to the console.

      This method is provided for convenience; it merely calls
      @link(AStream.printEach ConsoleOut.printEach).
    }
    function print(const items: array of const): TStreamIOSize; virtual;

    { Print the selected items to the diagnostic/error output.

      This method is provided for convenience; it merely calls
      @link(AStream.printEach ErrorOut.printEach).
    }
    function printError(const items: array of const): TStreamIOSize; virtual;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the stream used for console output.  The caller
      should NOT free this reference; that will be done when the application
      instance is freed.
    }
    function ConsoleOut: AStream; virtual;

    { Retrieve a reference to the stream used for console input.  The caller
      should NOT free this reference; that will be done with the application
      instance is freed.
    }
    function ConsoleIn: AStream; virtual;

    { Retrieve a reference to the stream used for diagnostic and error output.
      The caller should NOT free this reference; that will be done when the
      application instance is freed.
    }
    function ErrorOut: AStream; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// AConsoleApplication *********************************************************
// Initializer
function AConsoleApplication.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyConsoleOut := nil;
    MyConsoleIn := nil;
    MyErrorOut := nil;
  end;
end;

// Destructor
destructor AConsoleApplication.destroy;

begin
  // Free our console streams
  if MyErrorOut <> nil then
    MyErrorOut.free;

  if MyConsoleIn <> nil then
    MyConsoleIn.free;

  if MyConsoleOut <> nil then
    MyConsoleOut.free;

  inherited destroy;
end;

// Run the application
function AConsoleApplication.run: integer;

begin
  result := inherited run;

  if result <> 0 then
    exit;

  // Check to ensure our console streams have been created
  if MyConsoleOut = nil then
    MyConsoleOut := AStandardOutputStream.new;

  if MyErrorOut = nil then
    MyErrorOut := AStandardErrorStream.new;

  { Return non-zero (true) if any of the streams are nil.  Return zero (false)
    only if NONE of the streams are nil.
  }
  result := integer((MyConsoleOut = nil) or (MyErrorOut = nil));
end;

// Print the specified items to the console
function AConsoleApplication.print(const items: array of const): TStreamIOSize;

begin
  result := MyConsoleOut.printEach(items);
end;

// Print the specified items to error/diagnostic output
function AConsoleApplication.printError(const items: array of const): TStreamIOSize;

begin
  result := MyErrorOut.printEach(items);
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the stream used for console output
function AConsoleApplication.ConsoleOut: AStream;

begin
  Result := MyConsoleOut;
end;

// Retrieve a reference to the stream used for console input
function AConsoleApplication.ConsoleIn: AStream;

begin
  Result := MyConsoleIn;
end;

// Retrieve a reference to the stream used for diagnostic and error output
function AConsoleApplication.ErrorOut: AStream;

begin
  Result := MyErrorOut;
end;
{$ENDIF}

