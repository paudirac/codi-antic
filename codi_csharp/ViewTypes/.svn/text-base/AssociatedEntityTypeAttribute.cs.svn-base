using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HaleyMVCLib.ViewTypes
{
    /// <summary>
    /// Associates an EntityFramework Entity with a HaleyMVCLib TableView o EntityFormView
    /// subclass.
    /// </summary>
    [System.AttributeUsage(AttributeTargets.Class, AllowMultiple = true)]
    public class AssociatedEntityTypeAttribute : System.Attribute
    {
        public Type EntityType
        {
            get;
            private set;
        }

        public string ViewName
        {
            get;
            private set;
        }

        public AssociatedEntityTypeAttribute(Type entity_type, string view_name)
        {
            EntityType = entity_type;
            ViewName = view_name;
        }
    }
}
