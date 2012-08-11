using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HaleyMVCLib.ViewTypes
{
    /// <summary>
    /// A FlatRow is a Dictionary of string keys and object values, provided for
    /// the iteration and renderization of tables in views (in a loop, for instance).
    /// </summary>
    public class FlatRow : Dictionary<string, object>
    {
        public Key Key
        {
            get;
            private set;
        }

        public FlatRow(Key key)
        {
            Key = key;
        }
    }

    /// <summary>
    /// List of FlatRow.
    /// It's just a name for better compile-type checking and semantics.
    /// </summary>
    public class FlatRows : List<FlatRow>
    {
    }
}
