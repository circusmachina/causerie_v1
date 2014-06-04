{ streamwrappers.inc - A class to wrap around instances of AStream.

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
{ This class represents a wrapper around an instance of @link(AStream); that is,
  it defines the basis for a class which can manipulate and extend the
  capabilities of an arbitrary stream by acting as a stream itself.  The read
  and write methods are passed on to the underlying stream.

  The functionality provided by this class is fairly basic; it allows the caller
  to specify whether or not the wrapper inherits ownership  of the stream being
  wrapped, which means the class being wrapped will be freed when the wrapper is
  freed.  The base class defined here does not implement the abstract read and
  write methods defined by @link(AStream); that is for descendant classes to
  do.
}
type AStreamWrapper = class(AStream)
  protected
    { Refers to the target stream, around which this object wraps }
    MyTarget: AStream;
    { Indicates whether or not the wrapper owns its target }
    myOwnTarget: boolean;

  public
    { Initializer }
    function init: boolean; override;

    { Destroy the wrapper.

      If @link(AStreamWrapper.ownsTarget) is @true, then this method will also
      free the instance of the stream that it has wrapped.  Otherwise, it is
      for the calling routine to dispose of that instance when it is no longer
      required.
    }
    destructor destroy; override;

    { Read @code(count) bytes from the stream and store the resulting data in
      @code(dest).

      This method fails silently and returns zero (0).  It is for descendant
      classes to override the method if they support reading.

      @return(The total number of bytes read from the stream.  This will always
      be zero if called on an instance of @link(AStreamWrapper).)
    }
    function read(const count: TStreamIOSize; var dest): TStreamIOSize;
      override;

    { Write @code(count) bytes from @code(source) to the stream.

      This method fails silently and returns zero (0).  It is for descendant
      classes to override the method if they support writing.

      @return(The total number of bytes written to the stream.  This will always
      be zero if called on an instance of @link(AStreamWrapper).)
    }
    function write(const count: TStreamIOSize; const source): TStreamIOSize;
      override;

    { Retrieve the name of the stream.

      This method overrides the behavior inherited from @link(AStream): it
      calls @link(AStream.name) on the target stream and returns that value.
      This means that if the wrapper is working with an instance of
      @link(AFileStream), then calling this method will return the name of the
      file being streamed -- handy!
    }
    function name: string; override;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(AnObject.shallowCopyFrom): it calls that method, then checks to
      see whether @code(Other) is an instance of @link(AStreamWrapper).  If so,
      it copies the value of @link(AStreamWrapper.Target) from @code(Other) to
      @code(Self), overwriting any current values in @code(Self).

      This method does NOT copy the value of @link(AStreamWrapper.ownsTarget)
      from @code(Other); it always sets that value to @false for itself.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    // Properties --------------------------------------------------------------
    { Retrieve the current position within the stream.

      This method simply calls the corresponding method on the target stream
      and returns the value received to the caller.  Descendant classes may
      override this method to introduce different behavior.

      @return(The current position within the stream, as an offset in bytes from
      the beginning of the stream, which is always at offset zero (0).)
    }
    function position: TStreamOffset; override;

    { Set the current position within the stream.

      This method simply calls the corresponding method on the target stream
      and returns the value received to the caller.  Descendant classes may
      override this method to introduce different behavior.  For more
      information on what values @code(newPosition) may contain, see
      @link(AStream.setPosition).

      @return(The previous position within the stream, as an offset in bytes
      from the beginning of the stream, which is always at offset zero (0).)

    }
    function setPosition(newPosition: TStreamOffset): TStreamOffset; override;

    { Retrieve the current size of the stream, in bytes.

      This method simply calls the corresponding method on the target stream
      and returns the value received to the caller.  Descendant classes may
      override this method to introduce different behavior.

      @return(The current size of the stream, in bytes.)
    }
    function length: TStreamLength; override;

    { Retrieve a reference to the target stream.  The target stream is the
      stream for which the contents are being buffered.

      The reference returned by this function should NOT be freed by the caller.
      If @link(ABufferedStream.ownsTarget) is @true, then the buffered stream
      instance will free the target stream when it is itself freed.
    }
    function Target: AStream; virtual;

    { Set the target stream.  The target stream is the stream for which the
      contents are being buffered.

      This method should be used with care.  Descendant classes may override
      the behavior of this method so that the target stream cannot be
      arbitrarily changed after the stream wrapper is constructed; consult the
      documentation for those classes.

      If this method is used to alter the target stream, the caller should also
      call @link(AStreamWrapper.setOwnsTarget) to indicate whether or not the
      wrapper assumes ownership of the new target.  By default, this method
      sets @link(AStreamWrappers.ownsTarget) to @false when it is called.

      @return(The previous target, if any.  If this method does not return
      @nil, then it is the caller's responsibility to free the previous target
      stream in order to avoid memory leaks.)
    }
    function SetTarget(const NewTarget: AStream): AStream; virtual;

    { Determine whether or not the buffered stream instance owns its target.

      If the buffered stream instance owns its target, then it will free the
      target stream when it is itself freed.  Otherwise, it is up to the
      routine that constructed the buffered stream instance to free the target
      stream.
    }
    function ownsTarget: boolean; virtual;

    { Set whether or not the buffered stream instance owns its target.

      If the buffered stream instance owns its target, then it will free the
      target stream when it is itself freed.  Otherwise, it is up to the
      routine that constructed the buffered stream instance to free the target
      stream when it is no longer needed.

      @return(Whether or not the buffered stream instance previously owned its
      target.)
    }
    function setOwnsTarget(const flag: boolean): boolean; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// AStreamWrapper **************************************************************
// Initializer
function AStreamWrapper.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyTarget := nil;
    myOwnTarget := false;
  end;
end;

// Destructor
destructor AStreamWrapper.destroy;

begin
  // Free the target, if we own it
  if (MyTarget <> nil) and (myOwnTarget) then
    MyTarget.free;

  inherited destroy;
end;

// Read from the stream
function AStreamWrapper.read(const count: TStreamIOSize;
  var dest): TStreamIOSize;

begin
  // Not supported here; descendant classes must implement this
  result := 0;
end;

// Write to the stream
function AStreamWrapper.write(const count: TStreamIOSize;
  const source): TStreamIOSize;

begin
  // Not supported here; descendant classes must implement this
  result := 0;
end;

// Return the name of the stream
function AStreamWrapper.name: string;

begin
  result := inherited name;

  if MyTarget <> nil then
    result := MyTarget.name;
end;

// Construct a shallow copy of the given object
function AStreamWrapper.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other item is compatible with us
  if not(Other is AStreamWrapper) then
    result := false;

  if result then
  begin
    MyTarget := AStreamWrapper(Other).Target;
    // We do not automatically assume ownership of the target
    myOwnTarget := false;
  end;
end;

// Properties ------------------------------------------------------------------
// Get the current position in the stream
function AStreamWrapper.position: TStreamOffset;

begin
  result := 0;

  if MyTarget <> nil then
    result := MyTarget.position;
end;

// Set the current position in the stream
function AStreamWrapper.setPosition(newPosition: TStreamOffset): TStreamOffset;

begin
  result := 0;

  if MyTarget <> nil then
    result := MyTarget.setPosition(newPosition);
end;

// Get the current length of the stream
function AStreamWrapper.length: TStreamLength;

begin
  result := 0;

  if MyTarget <> nil then
    result := MyTarget.length;
end;

// Retrieve a reference to the target stream
function AStreamWrapper.Target: AStream;

begin
  Result := MyTarget;
end;

// Set the target stream
function AStreamWrapper.SetTarget(const NewTarget: AStream): AStream;

begin
  Result := MyTarget;
  MyTarget := NewTarget;

  // We do not automatically assume ownership of the target
  myOwnTarget := false;
end;

// Retrieve whether or not the buffered stream owns its target
function AStreamWrapper.ownsTarget: boolean;

begin
  result := myOwnTarget;
end;

// Set whether or not the buffered stream owns its target
function AStreamWrapper.setOwnsTarget(const flag: boolean): boolean;

begin
  result := myOwnTarget;
  myOwnTarget := flag;
end;
{$ENDIF}

