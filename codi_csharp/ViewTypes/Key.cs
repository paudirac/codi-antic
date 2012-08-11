using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HaleyMVCLib.ViewTypes
{
    // TODO: posar-li una Type KeyType?
    /// <summary>
    /// Key is a way of tracking the PrimaryKey of a row in a table (DB) within
    /// different requests. This is not a unique Entity Key. It is assumed it is
    /// a unique key in the table represented by the entity that is being updated.
    /// 
    /// Provides a handle for further changes (like multiple-column primary keys?).
    /// </summary>
    public class Key
    {
        public string Name { get; set; }
        public object Value { get; set; }
    }
}
