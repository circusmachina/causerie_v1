{ versioninfo.inc: A cross-platform method for obtaining application version
    information from the resources compiled into the current executable.

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
{ This class represents version information that is stored as a resource
  inside of an object file which represents the main executable of the current
  process.

  You will likely not create a direct instance of this class, as it has no
  real functionality.  It is defined solely to institute a common interface
  for application version information across platforms and object file types.
  Instead, you will likely use one of its descendants, such as
  @link(AnELFFileVersionInfoResource) or
  @link(AWindowsFileVersionInfoResource).
}
type AFileVersionInfoResource = class(AnObject)
  protected
    // Refers to the buffered raw version informaton
    myData: pchar;
    // Stores the size of the raw version information
    myLength: longword;
    // Stores the key/value pairs that express version information
    MyVersionInfo: ADictionary;

  public
    // Initializer
    function init: boolean; override;

    { Destroy the version info resource.

      If @code(AFileVersionInfoResource.data Self.data) is not @nil,
      this method will free the memory to which it refers before calling the
      inherited routine.
    }
    destructor destroy; override;

    { Read version information as key/value pairs from the object file, and
      store them in the specified dictionary.

      Descendant classes must implement this method.  The method should return
      @true if the operation was succesful and @false if it was not.
    }
    function readInto(const ThisDictionary: ADictionary): boolean; virtual;
      abstract;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the raw version info data.

      The caller should NOT attempt to free the memory to which the reference
      returned by this routine refers.  That will be done automatically when
      the file version info resource is freed.
    }
    function data: pointer; virtual;

    { Retrieve the size, in bytes, of the raw version data. }
    function length: longword; virtual;

    { Retrieve a reference to the key/value pairs that express the version
      information contained in the main executable of the current process.
    }
    function VersionInfo: ADictionary; virtual;
end;

{$IFNDEF WINDOWS}
{ This structure defines the items in the version resource which are fixed: that
  is, their sizes and locations within the resource are predefined.  Although
  this structure is defined by Microsoft for use with Windows, FPC and Lazarus
  encode it into ELF files as well.
}
type TVSFixedFileInfo = record
  dwSignature: longword;
  dwStrucVersion: longword;
  dwFileVersionMS: longword;
  dwFileVersionLS: longword;
  dwProductVersionMS: longword;
  dwProductVersionLS: longword;
  dwFileFlagsMask: longword;
  dwFileOS: longword;
  dwFileType: longword;
  dwFileSubtype: longword;
  dwFileDateMS: longword;
  dwFileDateLS: longword;
end;

// This type points to an instance of @link(TVSFixedFileInfo).
type PVSFixedFileInfo = ^TVSFixedFileInfo;

{$PACKRECORDS 2}
{ These elements exist at the beginning of every pseudo-structure. }
type TTypicalVersionInfoHeader = record
  wLength: word;
  wValueLength: word;
  wType: word;
end;
{$PACKRECORDS DEFAULT}

{ This type points to an instance of @link(TTypicalVersionInfoHeader). }
type PTypicalVersionInfoHeader = ^TTypicalVersionInfoHeader;

{ This class represents version information that is stored as a resource
  within the ELF object file that represents main code segment of the
  current process.  This information is automatically compiled into the
  ELF object file by Lazarus and FPC when the developer requests version
  information be included on compilation.

  This class is targeted specifically at ELF files, since Microsoft
  Windows provides the functions necessary to obtain version information
  from Windows executables.
}
type AnELFFileVersionInfoResource = class(AFileVersionInfoResource)
  protected
    // Stores the handle to the resource before it is loaded into memory
    myHandleFromFile: TFPResourceHandle;
    // Stores the handle to the resource
    myHandleInMemory: TFPResourceHGlobal;
    // Stores the length, in bytes, of any fixed version information
    myFixedInfoLength: word;
    // Stores the type of data stored in the version resource
    myDataType: word;
    // Stores the key that identifies valid valid version information
    myKey: WideString;
    // Stores fixed version information, if any
    myFixedInfo: PVSFixedFileInfo;

    { Read the @code(VS_VERSIONINFO) pseudo-structure.

      This method reads the @code(VS_VERSIONINFO) pseudo-structure from the raw
      resource data.  This pseudo-structure defines the overall attributes of
      the version information contained within the version resource.  It is
      called a pseudo-structure because its size and the position of some of its
      members cannot be known at compile-time.  Nonetheless, the structure
      begins with some elements whose sizes and positions are known; these are
      represented by the @link(TTypicalVersionInfoHeader) structure.

      This method checks that the @code(szKey) member of the pseudo-structure
      matches the value of @link(vsVersionInfoKey); this is one way of
      validating that the correct pseudo-structure is being read.  If the
      values do not match, the method exits immediately, returning @false.

      If the @code(wValueLength) member of the pseudo-structure has a value that
      is greater than zero (@code(0)), then this method will point
      @link(AnELFFileVersionInfoResource.fixedInfo Self.fixedInfo) at the
      data represented by the @code(Value) member of the pseudo-structure, which
      is described by @link(TVSFixedFileInfo).

      Finally, the method will call
      @link(AnELFFileVersionInfoResource.parseStringFileInfo) to parse the
      @code(StringFileInfo) structure that is contained in the @code(Children)
      pseudo-field of the pseudo-structure.  The return value of that call is
      returned by this method.

      @code(rawdata) must point to the raw resource data which will be used to
      parse the @code(VS_VERSIONINFO) pseudo-structure.  This pointer will be
      advanced as the structure is parsed.  If @code(rawdata) is @nil, then this
      method will do nothing and will return @false.
    }
    function parseVSVersionInfo(var rawdata: pchar): boolean; virtual;

    { This method reads the @code(StringFileInfo) pseudo-structure from the raw
      resource data.  This pseudo-structure defines the attributes of the
      string tables which make up the version information in the file.  There
      can be more than one string table in the version resource.

      The structure is called a pseudo-structure because its overall size and
      the position of some of its members cannot be known at compile-time.
      However, the structure begins with some elements whose sizes and positions
      are known; these are represented by the @link(TTypicalVersionInfoHeader)
      structure.

      This method checks that the @code(szKey) member of the pseudo-structure
      matches the value of @link(vsStringInfoKey); this is one way of validating
      that the correct pseudo-structure is being read.  If the values do not
      match, the method exits immediately, returning @false.

      Finally, the method will loop through calls to
      @link(AnELFFileVersionInfoResource.parseStringTable) to parse the array
      of @code(StringTable) pseudo-structures which are contained in the
      @code(Children) pseudo-field of the pseudo-structure.  If the call should
      return @false at any point in the loop, then the loop is broken.  The
      loop ends when @code(rawdata) passes beyond the end of the
      pseudo-structure's boundaries, as determined by comparing @code(rawdata)
      to the value of @code(rawdata + StringFileInfo.wLength).

      @code(rawdata) must point to the raw resource data which will be used to
      parse the @code(StringInfo) pseudo-structure.  The pointer will be
      advanced as the structure is parsed.  If @code(rawdata) is @nil, then this
      routine will do nothing and will return @false.
    }
    function parseStringFileInfo(var rawdata: pchar): boolean; virtual;

    { This method reads a @code(StringTable) pseudo-structure from the raw
      resource data.  This pseudo-structure defines the attributes of the
      key/value pairs which make up the version information in the file.  There
      can be more than one string table in the version resource; typically,
      there is one for each language and code page supported by the program.

      The structure is called a pseudo-structure because its overall size and
      the position of some of its members cannot be known at compile-time.
      However, the structure begins with some elements whose sizes and positions
      are known; these are represented by the @link(TTypicalVersionInfoHeader)
      structure.

      Finally, the method will loop through calls to
      @link(AnELFFileVersionInfoResource.parseString) to parse the array
      of @code(String) pseudo-structures which are contained in the
      @code(Children) pseudo-field of the pseudo-structure.  If the call should
      return @false at any point in the loop, then the loop is broken.  The
      loop ends when @code(rawdata) passes beyond the end of the
      pseudo-structure's boundaries, as determined by comparing @code(rawdata)
      to the value of @code(rawdata + StringTable.wLength).

      @code(rawdata) must point to the raw resource data which will be used to
      parse the @code(StringTable) pseudo-structure.  The pointer will be
      advanced as the structure is parsed.  If @code(rawdata) is @nil, then this
      routine will do nothing and will return @false.
    }
    function parseStringTable(var rawdata: pchar): boolean; virtual;

    { This method reads a @code(String) pseudo-structure from the raw
      resource data.  This pseudo-structure defines the key/value pairs which
      make up the version information in the file.  There are typicall several
      such pairs in the version resource.

      The structure is called a pseudo-structure because its overall size and
      the position of some of its members cannot be known at compile-time.
      However, the structure begins with some elements whose sizes and positions
      are known; these are represented by the @link(TTypicalVersionInfoHeader)
      structure.

      If the @code(Children) pseudo-field of the structure defines a
      non-empty string, then this method will insert the value of the
      @code(szKey) member of the pseudo-structure, along with the value of
      the @code(Children) pseudo-field, into
      @link(AFileVersionInfoResource.VersionInfo Self.VersionInfo).

      @code(rawdata) must point to the raw resource data which will be used to
      parse the @code(String) pseudo-structure.  The pointer will be
      advanced as the structure is parsed.  If @code(rawdata) is @nil, then this
      routine will do nothing and will return @false.
    }
    function parseString(var rawdata: pchar): boolean; virtual;

  public
    // Initializer
    function init: boolean; override;

    { Destroy the version resource.

      This method will call @code(System.unlockResource()) and
      @code(System.freeResource()) to release and free the raw resource data
      before calling the inherited routine.
    }
    destructor destroy; override;

    { Read version information from the resource information for the current
      executable.

      This method makes use of a few routines that are available in the
      FPC @code(System) unit, including
      @code(System.findResource()), @code(System.loadResource()),
      @code(System.sizeOfResource()) and @code(System.lockResource()).

      The method first calls @code(System.findResource()) to find the
      @code(RT_VERSION) resource in the current executable.  If the resource is
      found, the method then calls @code(System.loadResource()) to load the
      resource into memory, and then @code(System.sizeOfResource()) to determine
      its size and @code(System.lockResource()) to obtain access to the raw
      resource data.  It then calls
      @link(AnELFFileVersionInfoResource.parseVSVersionInfo) to parse the
      @link(TVSVersionInfo) pseudo-structure from the resource data.

      @return(@true if the version information was successfully loaded and
        parsed; @false otherwise.  If the version information was parsed
        successfully, the keys and values that make up the version information
        will be added to @code(ThisDictionary).  If @code(ThisDictionary) is
        @nil, this routine will do nothing and will return @false.)
    }
    function readInto(const ThisDictionary: ADictionary): boolean; override;

    // Properties --------------------------------------------------------------
    { Retrieve the length, in bytes, of any fixed information in the resource }
    function fixedInfoLength: word; virtual;

    { Retrieve the type of data stored in the version resource.

      The value returned by this method will be zero (@code(0)) if the
      resource contains binary data and one (@code(1)) if the resource
      contains strings.
    }
    function dataType: word; virtual;

    // Retrieve the key that identifies valid version information
    function key: WideString; virtual;
    // Retrieve a reference to the fixed version information, if any
    function fixedInfo: PVSFixedFileInfo; virtual;
end;

const
  { Microsoft's version pseudo-structures are aligned to 32-bit addresses. }
  VS_PADDING_ALIGNMENT = sizeof(longword);
  { Microsoft's version pseudo-structures are alingted to 32-bit addresses by
    by using zero-value words.
  }
  VS_PADDING_INCREMENT = sizeof(word);

  { This string value is used as a key to validate the
    @code(VS_VERSIONINFO) pseudo-structure.
  }
  vsVersionInfoKey: WideString = 'VS_VERSION_INFO';

  { This string value is used as a key to validate the @code(StringFileInfo)
    pseudo-structure.
  }
  vsStringFileInfoKey: WideString = 'StringFileInfo';

{$ELSE}
{ This class represents version information that is stored as a resource
  within the Windows object file that represents main code segment of the
  current process.  This information is automatically compiled into the
  Windows object file by Lazarus and FPC when the developer requests version
  information be included on compilation.

  This class is targeted specifically at Windows executables.
}
type AWindowsFileVersionInfoResource = class(AFileVersionInfoResource)
  public
    { Read version information from the resource information for the current
      executable.

      This method iterates through an array of known resource keys names and
      calls @code(Windows.verQueryValue) in an attempt to obtain the value for
      each of them.  The ones that are found are added to @code(ThisDictionary)
      using the name of the resource key which was found.

      @return(@true if version information was found and stored in the
        specified dictionary; @false otherwise.  If @code(ThisDictionary) is
        @nil, this routine will do nothing and will return @false.)
    }
    function readInto(const ThisDictionary: ADictionary): boolean; override;
end;
{$ENDIF}

{$ENDIF}

{$IFDEF ReadResourceStrings}
  { This value represents one of the possible keys that can be stored inside of
    a version information resource.  It is one of several keys that, if found
    in the version resource, will be added -- along with the corresponding
    value -- to the dictionary specified when
    @link(AFileVersionInfoResource.readInto) is called.
  }
  vinfComments = 'Comments';

  { This value represents one of the possible keys that can be stored inside of
    a version information resource.  It is one of several keys that, if found
    in the version resource, will be added -- along with the corresponding
    value -- to the dictionary specified when
    @link(AFileVersionInfoResource.readInto) is called.
  }
  vinfCompanyName = 'CompanyName';

  { This value represents one of the possible keys that can be stored inside of
    a version information resource.  It is one of several keys that, if found
    in the version resource, will be added -- along with the corresponding
    value -- to the dictionary specified when
    @link(AFileVersionInfoResource.readInto) is called.
  }
  vinfFileDescription = 'FileDescription';

  { This value represents one of the possible keys that can be stored inside of
    a version information resource.  It is one of several keys that, if found
    in the version resource, will be added -- along with the corresponding
    value -- to the dictionary specified when
    @link(AFileVersionInfoResource.readInto) is called.
  }
  vinfFileVersion = 'FileVersion';

  { This value represents one of the possible keys that can be stored inside of
    a version information resource.  It is one of several keys that, if found
    in the version resource, will be added -- along with the corresponding
    value -- to the dictionary specified when
    @link(AFileVersionInfoResource.readInto) is called.
  }
  vinfInternalName = 'InternalName';

  { This value represents one of the possible keys that can be stored inside of
    a version information resource.  It is one of several keys that, if found
    in the version resource, will be added -- along with the corresponding
    value -- to the dictionary specified when
    @link(AFileVersionInfoResource.readInto) is called.
  }
  vinfLegalCopyright = 'LegalCopyright';

  { This value represents one of the possible keys that can be stored inside of
    a version information resource.  It is one of several keys that, if found
    in the version resource, will be added -- along with the corresponding
    value -- to the dictionary specified when
    @link(AFileVersionInfoResource.readInto) is called.
  }
  vinfLegalTrademarks = 'LegalTrademarks';

  { This value represents one of the possible keys that can be stored inside of
    a version information resource.  It is one of several keys that, if found
    in the version resource, will be added -- along with the corresponding
    value -- to the dictionary specified when
    @link(AFileVersionInfoResource.readInto) is called.
  }
  vinfOriginalFilename = 'OriginalFileName';

  { This value represents one of the possible keys that can be stored inside of
    a version information resource.  It is one of several keys that, if found
    in the version resource, will be added -- along with the corresponding
    value -- to the dictionary specified when
    @link(AFileVersionInfoResource.readInto) is called.
  }
  vinfPrivateBuild = 'PrivateBuild';

  { This value represents one of the possible keys that can be stored inside of
    a version information resource.  It is one of several keys that, if found
    in the version resource, will be added -- along with the corresponding
    value -- to the dictionary specified when
    @link(AFileVersionInfoResource.readInto) is called.
  }
  vinfProductName = 'ProductName';

  { This value represents one of the possible keys that can be stored inside of
    a version information resource.  It is one of several keys that, if found
    in the version resource, will be added -- along with the corresponding
    value -- to the dictionary specified when
    @link(AFileVersionInfoResource.readInto) is called.
  }
  vinfProductVersion = 'ProductVersion';

  { This value represents one of the possible keys that can be stored inside of
    a version information resource.  It is one of several keys that, if found
    in the version resource, will be added -- along with the corresponding
    value -- to the dictionary specified when
    @link(AFileVersionInfoResource.readInto) is called.
  }
  vinfSpecialBuild = 'SpecialBuild';
{$ENDIF}

{$IFDEF ReadImplementation}
const
  // This constant indicates how many version info keys there are
  versionInfoKeyCount = 12;

  // This array makes it easy to iterate through possible version info keys
  versionInfoKeys: array[0..(versionInfoKeyCount - 1)] of AnsiString = (
    vinfComments, vinfCompanyName,
    vinfFileDescription, vinfFileVersion,
    vinfInternalName,
    vinfLegalCopyright, vinfLegalTrademarks,
    vinfOriginalFilename,
    vinfPrivateBuild, vinfProductName, vinfProductVersion,
    vinfSpecialBuild
  );

// AFileVersionInfoResource ****************************************************
// Initializer
function AFileVersionInfoResource.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myData := nil;
    myLength := 0;
  end;
end;

// Destructor
destructor AFileVersionInfoResource.destroy;

begin
  // Free the raw version data, if any
  if myData <> nil then
    System.freeMem(myData);

  inherited destroy;
end;

// Properties ------------------------------------------------------------------
// Retrieve a pointer to the raw resource data in memory
function AFileVersionInfoResource.data: pointer;

begin
  result := myData;
end;

// Retrieve the length, in bytes, of the version resource
function AFileVersionInfoResource.length: longword;

begin
  result := myLength;
end;

// Retrieve a reference to the key/value pairs associated with the resource
function AFileVersionInfoResource.VersionInfo: ADictionary;

begin
  Result := MyVersionInfo;
end;

{$IFNDEF WINDOWS}

// AFileVersionInfoResource ****************************************************
// Initializer
function AnELFFileVersionInfoResource.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myHandleFromFile := 0;
    myHandleInMemory := 0;
    myFixedInfoLength := 0;
    myDataType := 0;
    myKey := '';
    myFixedInfo := nil;
  end;
end;

// Destructor
destructor AnELFFileVersionInfoResource.destroy;

begin
  // Release the raw resource data, if any
  if myHandleInMemory <> 0 then
  begin
    System.unlockResource(myHandleInMemory);
    System.freeResource(myHandleInMemory);

    { Prevent the inherited routine from attempting to free memory which is now
      no longer valid
    }
    myData := nil;
  end;

  inherited destroy;
end;

// Parse the VS_VERSIONINFO pseudo-structure
function AnELFFileVersionInfoResource.parseVSVersionInfo(var rawdata: pchar): boolean;

var
  // Refers to the header of the pseudo-structure
  structureHeader: PTypicalVersionInfoHeader;

begin
  result := false;

  if rawdata = nil then
    exit;

  // Get the typical header
  structureHeader := PTypicalVersionInfoHeader(rawdata);
  inc(rawdata, sizeof(TTypicalVersionInfoHeader));

  // Get the value of wLength
  myLength := structureHeader^.wLength;
  // Get the value of wValueLength
  myFixedInfoLength := structureHeader^.wValueLength;
  // Get the value of wType
  myDataType := structureHeader^.wType;

  // Retrieve the value of szKey
  myKey := pWideChar(rawdata);
  // Include NULL terminator
  inc(rawdata, (System.length(myKey) + 1) * sizeof(widechar));

  // Check the key
  if myKey <> vsVersionInfoKey then
    exit;

{$IFDEF DEBUG}
  writeln('Read VS_VERSIONINFO pseudo-structure:');
  writeln(#9'wLength: ', structureHeader^.wLength);
  writeln(#9'wValueLength: ', structureHeader^.wValueLength);
  writeln(#9'wType: ', structureHeader^.wType);
  writeln(#9'Key: ', myKey);
{$ENDIF}

  // Allow for any padding
  inc(rawdata, MemorySpan.paddingRequired(rawdata, myData, VS_PADDING_ALIGNMENT,
    VS_PADDING_INCREMENT));

  // Get the value of Value, if any
  if myFixedInfoLength > 0 then
  begin
    myFixedInfo := PVSFixedFileInfo(rawdata);
    inc(rawdata, myFixedInfoLength);
  end;

  // Have we reached the end of the pseudo-structure?
  if rawdata >= (myData + myLength) then
    exit;

  // Allow for further padding
  inc(rawdata, MemorySpan.paddingRequired(rawdata, myData, VS_PADDING_ALIGNMENT,
    VS_PADDING_INCREMENT));

  if rawdata < (myData + myLength) then
    result := Self.parseStringFileInfo(rawdata);
end;

// Parse the StringFileInfo pseudo-structure
function AnELFFileVersionInfoResource.parseStringFileInfo(var rawdata: pchar): boolean;

var
  // Refers to the header of the pseudo-structure
  structureHeader: PTypicalVersionInfoHeader;
  // Stores the key that identifies the pseudo-structure
  szKey: WideString;

begin
  result := false;

  if rawdata = nil then
    exit;

  // Get the structure header
  structureHeader := PTypicalVersionInfoHeader(rawdata);
  inc(rawdata, sizeof(TTypicalVersionInfoHeader));

  // Retrieve the key that identifies the pseudo-structure
  szKey := pWideChar(rawdata);
  // Include NULL terminator
  inc(rawdata, (System.length(szKey) + 1) * sizeof(wideChar));

  // Check the key
  if szKey <> vsStringfileInfoKey then
    exit;

  // Allow for padding
  inc(rawdata, MemorySpan.paddingRequired(rawdata, pchar(structureHeader),
    VS_PADDING_ALIGNMENT, VS_PADDING_INCREMENT));

{$IFDEF DEBUG}
  writeln('Read StringFileInfo pseudo-structure:');
  writeln(#9'wLength: ', structureHeader^.wLength);
  writeln(#9'wValueLength: ', structureHeader^.wValueLength);
  writeln(#9'wType: ', structureHeader^.wType);
  writeln(#9'Key: ', szKey);
{$ENDIF}

  // Have we reached the end of the pseudo-structure?
  while rawdata < (pchar(structureHeader) + structureHeader^.wLength) do
  begin
    result := Self.parseStringTable(rawdata);

    if result = false then
      break;
  end;
end;

// Parse the StringTable pseudo-structure
function AnELFFileVersionInfoResource.parseStringTable(var rawdata: pchar): boolean;

var
  // Refers to the header of the pseudo-structure
  structureHeader: PTypicalVersionInfoHeader;
  // Stores the key that identifies the pseudo-structure
  szKey: WideString;

begin
  result := false;

  if rawdata = nil then
    exit;

  // Get the structure header
  structureHeader := PTypicalVersionInfoHeader(rawdata);
  inc(rawdata, sizeof(TTypicalVersionInfoHeader));

  // Get the value of szKey
  szKey := pWideChar(rawdata);
  // Include NULL terminator
  inc(rawdata, (System.length(szKey) + 1) * sizeof(wideChar));

  // Allow for padding
  inc(rawdata, MemorySpan.paddingRequired(rawdata, pchar(structureHeader),
    VS_PADDING_ALIGNMENT, VS_PADDING_INCREMENT));

{$IFDEF DEBUG}
  writeln('Read StringTable pseudo-structure:');
  writeln(#9'wLength: ', structureHeader^.wLength);
  writeln(#9'wValueLength: ', structureHeader^.wValueLength);
  writeln(#9'wType: ', structureHeader^.wType);
  writeln(#9'Key: ', szKey);
{$ENDIF}

  // Have we reached the end of the pseudo-structure?
  while rawdata < (pchar(structureHeader) + structureHeader^.wLength) do
  begin
    result := Self.parseString(rawdata);

    if result = false then
      break;
  end;
end;

// Parse the String pseudo-structure
function AnELFFileVersionInfoResource.parseString(var rawdata: pchar): boolean;

var
  // Refers to the structure header
  structureHeader: PTypicalVersionInfoHeader;
  // Stores the key associated with the string
  szKey: WideString;
  // Stores the value associated with the structure
  value: WideString;

begin
  result := false;

  if (MyVersionInfo = nil) or (rawdata = nil) then
    exit;

  // Get the structure header
  structureHeader := PTypicalVersionInfoHeader(rawdata);
  inc(rawdata, sizeof(TTypicalVersionInfoHeader));

  // Read the key associated with the string
  szKey := pWideChar(rawdata);
  // Include NULL terminator
  inc(rawdata, (System.length(szKey) + 1) * sizeof(wideChar));

  // Allow for padding
  inc(rawdata, MemorySpan.paddingRequired(rawdata, pchar(structureHeader),
    VS_PADDING_ALIGNMENT, VS_PADDING_INCREMENT));

  // Read the string
  value := pWideChar(rawdata);
  // Include NULL terminator
  inc(rawdata, structureHeader^.wValueLength * sizeof(wideChar));

  { Allow for padding.  This isn't documented on Microsoft's website, but the
    next String pseudo-structure must also be aligned on a 32-byte boundary.
    Found out through trial and error!
  }
  inc(rawdata, MemorySpan.paddingRequired(rawdata, pchar(structureHeader),
    VS_PADDING_ALIGNMENT, VS_PADDING_INCREMENT));

{$IFDEF DEBUG}
  writeln('Read String pseudo-structure:');
  writeln(#9'wLength: ', structureHeader^.wLength);
  writeln(#9'wValueLength: ', structureHeader^.wValueLength * sizeof(wideChar));
  writeln(#9'wType: ', structureHeader^.wType);
  writeln(#9'Key: ', szKey);
  writeln(#9'Value: ', value);
{$ENDIF}

  // Store the key/value pair
  if value <> '' then
    MyVersionInfo.setValueOf(szKey, value);

  result := true;
end;

// Read the version information for the current process
function AnELFFileVersionInfoResource.readInto(const ThisDictionary: ADictionary): boolean;

var
  // Refers to the data being parsed
  recentData: pchar;

begin
  result := false;

  if ThisDictionary = nil then
    exit;

  MyVersionInfo := ThisDictionary;

  // Find the version information in the file
  myHandleFromFile := System.findResource(0, pchar(1), RT_VERSION);
  if myHandleFromFile = 0 then
    exit;

  // Load the resource from disk
  myHandleInMemory := System.loadResource(0, myHandleFromFile);
  if myHandleInMemory = 0 then
    exit;

  // Determine the size of the resource
  myLength := System.sizeOfResource(0, myHandleFromFile);

  // Lock the resource
  myData := System.lockResource(myHandleInMemory);
  recentData := myData;

  // Parse the version info
  result := Self.parseVSVersionInfo(recentData);
end;

// Properties ------------------------------------------------------------------
// Retrieve the length, in bytes, of any fixed information in the resource
function AnELFFileVersionInfoResource.fixedInfoLength: word;

begin
  result := myFixedInfoLength;
end;

// Retrieve the type of data stored in the version resource
function AnELFFileVersionInfoResource.dataType: word;

begin
  result := myDataType;
end;

// Retrieve the key that identifies valid version information
function AnELFFileVersionInfoResource.key: WideString;

begin
  result := myKey;
end;

// Retrieve a reference to the fixed version information, if any
function AnELFFileVersionInfoResource.fixedInfo: PVSFixedFileInfo;

begin
  result := myFixedInfo;
end;

{$ELSE}

const
  { This is the hierarchical prefix that must be prepended to version info key
    names when querying for the values of a specific key.
  }
  vinfPrefix = 'StringFileInfo\040904E4\';

// AWindowsFileVersionResource *************************************************
// Read version information from the executable
function AWindowsFileVersionInfoResource.readInto(const ThisDictionary: ADictionary): boolean;

var
  // Stores the name of the current executable
  executableName: AnsiString;
  // Stores the full name of the desired key
  desiredKey: AnsiString;
  // Stores the value of the desired key
  value: AnsiString;
  // Points to the value of the desired key
  pValue: pchar;
  // Stores the length of the value
  valueLength: longword;
  // Loop counter
  i: longword;

begin
  result := false;

  if ThisDictionary = nil then
    exit;

  MyVersionInfo := ThisDictionary;

  // Get the executable name and attempt to read the version resource
  executableName := paramStr(0);
  myLength := Windows.getFileVersionInfoSize(@executableName[1], myLength);
  if myLength = 0 then
    exit;

  // Allocate memory for the raw data
  myData := System.getMem(myLength);
  if myData = nil then
    exit;

  // Read the data
  if Windows.getFileVersionInfo(@executableName[1], 0, myLength, myData) = false then
    exit;

  // Query for the various version values
  for i := 0 to (System.length(versionInfoKeys) - 1) do
  begin
    value := '';

    desiredKey := vinfPrefix + versionInfoKeys[i];

    if Windows.verQueryValue(myData, @desiredKey[1], pValue, valueLength) then
    begin
      value := pValue;
      MyVersionInfo.setValueOf(versionInfoKeys[i], value);
    end;
  end;

  result := MyVersionInfo.census > 0;
end;

{$ENDIF}

{$ENDIF}
