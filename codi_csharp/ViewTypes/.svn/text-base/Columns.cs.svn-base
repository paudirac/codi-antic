using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HaleyMVCLib.ViewTypes
{
    /// <summary>
    /// Stores Column information for the automatic or custom generation
    /// of tabular data views.
    /// </summary>
    public class ColumnInfo
    {
        public string Name { get; set; }
        public string Caption { get; set; }
        public string UIHint { get; set; }
        public bool Hide { get; set; }
        public FieldTypes FieldType { get; set; }

        public ColumnInfo(FieldTypes fieldType)
        {
            FieldType = fieldType;
        }
    }

    public class Columns : List<ColumnInfo>
    {
    }
}
