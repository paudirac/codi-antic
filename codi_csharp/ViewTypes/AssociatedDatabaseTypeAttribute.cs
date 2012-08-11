using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HaleyMVCLib.ViewTypes
{
    /// <summary>
    /// Associates an Entity Framework ObjectContext object with a
    /// HaleyMVCLib TableView or EntityFormView subclass.
    /// </summary>
    [System.AttributeUsage(AttributeTargets.Class, AllowMultiple = false)]
    public class AssociatedDatabaseTypeAttribute : System.Attribute
    {
        public Type DatabaseType
        {
            get;
            private set;
        }

        public AssociatedDatabaseTypeAttribute(Type database_type)
        {
            DatabaseType = database_type;
        }
    }
}
