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
// CanEvaluateAsInteger ********************************************************
{ This interface is adopted by those classes that can return integer
  representations of their data.
}
type CanEvaluateAsInteger = interface
  { Return an integer representation of the instance. }
  function toInteger: int64;
end;

// CanEvaluateAsDouble *********************************************************
{ This interface is adopted by those classes that can return floating-point
  representations of their data.
}
type CanEvaluateAsDouble = interface
  { Return a floating-point representation of the instance. }
  function toDouble: double;
end;

// AComparableObject ***********************************************************
{ This class serves as the ancestor of all classes whose instances retain
  values that can be compared with the values of other instances.

  Note that this class does not declare itself as implementing either
  @link(CanEvaluateAsInteger) or @link(CanEvaluateAsDouble); that is for
  descendant classes to do.  The methods defined by this class will only
  work with instances that implement one of the two interfaces.  If an
  instance implements both, the implementation of @code(CanEvaluateAsInteger)
  takes precedence.
}
type AComparableObject = class(APrintingObject)
  public
    { Compare the value of the current instance against that specified by
      @code(Other).

      The base implementation of this method checks to ensure that both
      @code(Self) and @code(Other) implement either @link(CanEvaluateAsInteger)
      or @link(CanEvaluateAsDouble).  Both must implement at least one of
      the interfaces; otherwise, the routine will always return 1, indicating
      that @code(Self) is not equal to @code(Other).

      If both instances implement one of the above interfaces, then the routine
      will compute and return the difference between the two instances, as
      specified by calls to either @link(CanEvaluateAsInteger.toInteger) or
      @link(CanEvaluateAsDouble.toDouble), as appropriate.  A value of zero (0)
      means that both instances evaluate as equals; a negative value indicates
      that @code(Other) has a value that is greater than @code(Self), while
      a positive value indicates that @code(Self) has a value that is greater
      than @code(Other).

      When one or both instances can only be evaluated as floating-point values,
      then the value returned by this routine will only approximate the
      difference between the two, since the result of calculating the difference
      is truncated to an integer value by means of @code(System.round).  For
      example, if one of the instances were to evaluate as @code(0.5) and the
      other as @code(0), then based on the rules of @code(System.round), they
      would both be considered equal, since @code(System.round) rounds toward
      the even value (zero, in this case).
    }
    function comparedWith(const Other: AnObject): int64; virtual;

    { Determine whether the value of the current instance is equal to that
      specified by @code(Other).

      This method simply calls @link(AComparableObject.comparedWith) and
      returns @true if that routine returns zero (0) and @false if it returns
      a non-zero value.
    }
    function isEqualTo(const Other: AnObject): boolean; virtual;

    { Determine whether the value of the current instance is not equal to that
      specified by @code(Other).

      This method simply calls @link(AComparableObject.comparedWith) and
      returns @true if that routine returns a non-zero value and @false if
      it returns a zero (0) value.
    }
    function isNotEqualTo(const Other: AnObject): boolean; virtual;

    { Determine whether the value of the current instance is greater than that
      specified by @code(Other).

      This method simply calls @link(AComparableObject.comparedWith) and
      returns @true if that routine returns a positive value and @false if it
      does not.
    }
    function isGreaterThan(const Other: AnObject): boolean; virtual;

    { Determine whether the value of the current instance is greater than or
      equal to that specified by @code(Other).

      This method simply calls @link(AComparableObject.comparedWith) and
      returns @true if that routine returns a positive value or zero (0) and
      @false otherwise.
    }
    function isGreaterThanOrEqualTo(const Other: AnObject): boolean; virtual;

    { Determine whether the value of the current instance is less than that
      specified by @code(Other).

      This method simply calls @link(AComparableObject.comparedWith) and
      returns @true if that routine returns a negative value and @false
      otherwise.
    }
    function isLessThan(const Other: AnObject): boolean; virtual;

    { Determine whether the value of the current instance is less than or
      equal to that specified by @code(Other).

      This method simply calls @link(AComparableObject.comparedWith) and
      returns @true if that routine returns a negative value or zero (0) and
      @false otherwise.
    }
    function isLessThanOrEqualTo(const Other: AnObject): boolean; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// AComparableObject ***********************************************************
/// Compare the value of the current instance with another
function AComparableObject.comparedWith(const Other: AnObject): int64;

begin
  result := 1;

  if Other = nil then
    exit;

  if (Self is CanEvaluateAsInteger) and (Other is CanEvaluateAsInteger) then
    result := (Self as CanEvaluateAsInteger).toInteger -
      (Other as CanEvaluateAsInteger).toInteger

  else if (Self is CanEvaluateAsInteger) and (Other is CanEvaluateAsDouble) then
    result := round((Self as CanEvaluateAsInteger).toInteger -
      (Other as CanEvaluateAsDouble).toDouble)

  else if (Self is CanEvaluateAsDouble) and (Other is CanEvaluateAsInteger) then
    result := round((Self as CanEvaluateAsDouble).toDouble -
      (Other as CanEvaluateAsInteger).toInteger)

  else if (Self is CanEvaluateAsDouble) and (Other is CanEvaluateAsDouble) then
    result := round((Self as CanEvaluateAsDouble).toDouble -
      (Other as CanEvaluateAsInteger).toInteger);
end;

/// Determine if the two values are equal
function AComparableObject.isEqualTo(const Other: AnObject): boolean;

begin
  result := (Self.comparedWith(Other) = 0);
end;

/// Determine if the two values are unequal
function AComparableObject.isNotEqualTo(const Other: AnObject): boolean;

begin
  result := (Self.comparedWith(Other) <> 0);
end;

/// Determine if the value of the current instance is greater than the other
function AComparableObject.isGreaterThan(const Other: AnObject): boolean;

begin
  result := (Self.comparedWith(Other) > 0);
end;

/// Determine if the value of the current instance is greater or equal to the other
function AComparableObject.isGreaterThanOrEqualTo(const Other: AnObject): boolean;

begin
  result := (Self.comparedWith(Other) >= 0);
end;

/// Determine if the value of the current instance is less than the other
function AComparableObject.isLessThan(const Other: AnObject): boolean;

begin
  result := (Self.comparedWith(Other) < 0);
end;

/// Determine if the value of the current instance is less than or equal to the other
function AComparableObject.isLessThanOrEqualTo(const Other: AnObject): boolean;

begin
  result := (Self.comparedWith(Other) <= 0);
end;
{$ENDIF}

