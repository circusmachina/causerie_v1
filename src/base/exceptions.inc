{ This file is part of classwork.pp, a library of basic and useful classes.

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
{ A basic exception.

  This is the base class used by all exceptions in causerie.  It provides
  methods for constructing and printing readable error messages to both
  @code(stderr) and instances of AStream.
}
type AnException = class (APrintingObject)
  protected
    { Stores the error code, if any, associated with the exception }
    myCode: longword;
    { Refers to the exception mesage text.

      This text is printed as part of the message output when
      @link(AnException.print) or @link(AnException.toString) is called.
    }
    myMessage: string;

  public
    { Constructs a new instance of AnException with the specified error code.

      The error message stored by the new instance is set to
      @link(errDefaultMessage).
    }
    constructor because(errorCode: integer);

    { Constructs a new instance of AnException with the specified message.

      The error code stored by the new instance is set to zero (0).
    }
    constructor because(const msg: string);

    { Constructs a new instance of AnException with the specified error code
      and message.
    }
    constructor because(errorCode: integer; const msg: string);

    { Constructs a new instance of AnException using the specified format
      string and details.

      @code(msg) should be a format string that, when combined with the
      contents of @code(details), will result in an error message that indicates
      the nature of the exception.
    }
    constructor because(const msg: string; details: array of const);

    { Constructs a new instance of AnException using the specified error code,
      format string, and details.

      @code(msg) should be a format string that, when combined with the
      contents of @code(details), will result in an error message which
      indicates the nature of the exception.
    }
    constructor because(errorCode: integer; const msg: string;
      details: array of const);

    { Initializer }
    function init: boolean; override;

    { Writes the exception to @code(stderr).

      This method calls @link(AnException.toString) and prints the result of
      that call to @code(stderr).
    }
    procedure print;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(AnObject.shallowCopyFrom): it calls that method, then checks to
      see whether @code(Other) is an instance of @link(AnException).  If so,
      it copies the values of:
      @unorderedList(
        @item(@link(AnException.code) and)
        @item(@link(AnException.message))
      )
      to @code(Self) from @code(Other), overwriting any values in @code(Self).
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

  // CanPrint implementation ---------------------------------------------------
    { Constructs and returns a string representation of the exception,
      suitable for printing to a stream or display.

      The base implementation of this method will construct a string that
      contains the error code, if any, as well as the error message associated
      with the exception.  Descendant classes may override this method to
      produce different output.
    }
    function toString: string; override;

  // Properties ---------------------------------------------------------------
    { Retrieves the error code associated with the exception. }
    function code: longword;

    { Retrieves the error message associated with the exception.

      The base implementation of this method does NOT return the fully-formed
      string that is returned by @link(AnException.toString); it simply returns
      the error message as defined during construction, or the default error
      message if none was provided when the instance was constructed.
    }
    function message: string; virtual;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string governs the output of @link(AnException.toString) when an
    error code was passed to the associated exception on construction.
  }
  errStringRepresentationWithCode = '(%d): %s' + lineEnding;

  { This string governs the output of @link(AnException.toString) when no
    error code was passed to the associated exception on construction.
  }
  errStringRepresentation = '%s' + lineEnding;

  { This string specifies the default error message that is used when no other
    message is passed during construction to an instance of AnException.
  }
  errDefaultMessage = 'an error has occurred';
{$ENDIF}
{$IFDEF ReadImplementation}
// AnException *****************************************************************
// Constructor
constructor AnException.because(errorCode: integer);

begin
  inherited new;
  myCode := errorCode;
end;

// Constructor
constructor AnException.because(const msg: string);

begin
  inherited new;
  myMessage := msg;
end;

// Constructor
constructor AnException.because(errorCode: integer; const msg: string);

begin
  inherited new;
  myCode := errorCode;
  myMessage := msg;
end;

// Constructor
constructor AnException.because(const msg: string; details: array of const);

begin
  inherited new;
  myMessage := Sysutils.format(msg, details);
end;

// Constructor
constructor AnException.because(errorCode: integer; const msg: string;
  details: array of const);

begin
  // Initialize the new instance
  inherited new;
  // Store the error code
  myCode := errorCode;
  // Store the error message
  myMessage := format(msg, details);
end;

// Initializer
function AnException.init: boolean;

begin
  // No error code as yet
  myCode := 0;
  // Use the default error message for now
  myMessage := errDefaultMessage;
  result := inherited init;
end;

// Print the result of AnException.toString to stdout
procedure AnException.print;

begin
  write(stdErr, Self.toString);
end;

// Construct a shallow copy of the given object
function AnException.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other item is compatible with us
  if not(Other is AnException) then
    result := false;

  if result then
  begin
    myCode := AnException(Other).code;
    myMessage := AnException(Other).message;
  end;
end;

// CanPrint implementation -----------------------------------------------------
// Construct a string representation of the exception.
function AnException.toString: string;

begin
  // If we have an error code, then include it in the output
  if myCode <> 0 then
    result := format(errStringRepresentationWithCode, [myCode, myMessage])

  // Otherwise, output the message only
  else
    result := format(errStringRepresentation, [myMessage]);
end;

// Properties ------------------------------------------------------------------
// Retrieves the code associated with the exception
function AnException.code: longword;

begin
  result := myCode;
end;

// Retrieves the message associated with the exception
function AnException.message: string;

begin
  result := myMessage;
end;
{$ENDIF}

