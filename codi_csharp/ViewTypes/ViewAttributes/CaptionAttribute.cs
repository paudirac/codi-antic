using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HaleyMVCLib.ViewTypes.ViewAttributes
{
    /// <summary>
    /// Caption to be displayed in forms or as the column name in tables.
    /// </summary>
    [System.AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]    
    public class CaptionAttribute : System.Attribute
    {
        public string Caption
        {
            get;
            private set;
        }

        public CaptionAttribute(string caption)
        {
            Caption = caption;
        }
    }
}
