using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HaleyMVCLib.ViewTypes.ViewAttributes
{
    /// <summary>
    /// Associates an ASP.NET MVC PartialView to a Field.
    /// 
    /// NOTE: currently, this PartialView must be located
    /// at: ~/Views/Shared/FieldTemplates/ folder.
    /// </summary>
    [System.AttributeUsage(AttributeTargets.Field)]
    public class UIHintAttribute : System.Attribute
    {
        public string RenderTo { get; private set; }
        public UIHintAttribute(string renderTo)
        {
            RenderTo = renderTo;
        }
    }
}
