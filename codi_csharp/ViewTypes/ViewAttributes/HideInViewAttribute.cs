using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HaleyMVCLib.ViewTypes.ViewAttributes
{
    /// <summary>
    /// Makes the decorated field not visible in forms or tables.
    /// </summary>
    [System.AttributeUsage(AttributeTargets.Field)]
    public class HideInViewAttribute : System.Attribute
    {
    }
}
