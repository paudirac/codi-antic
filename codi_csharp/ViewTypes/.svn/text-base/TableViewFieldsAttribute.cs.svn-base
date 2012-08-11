using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HaleyMVCLib.ViewTypes
{
    [System.AttributeUsage(AttributeTargets.Class, AllowMultiple = false)]
    public class TableViewFieldsAttribute : System.Attribute
    {
        public List<string> Fields
        {
            get;
            private set;
        }

        public TableViewFieldsAttribute(string[] columns)
        {
            Fields = new List<string>(columns);
        }
    }
}
