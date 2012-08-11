using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using HaleyMVCLib.Validators;

namespace HaleyMVCLib.ViewTypes
{
    /// <summary>
    /// FormField represents a form field. :S
    /// It carries information about the Value, and "meta"-information about
    /// the Caption, Name (the field name in the Entity), the validation information
    /// in IsValid, ErrorMessage, and the FieldType.
    /// </summary>
    public class FormField
    {
        public string Name { get; set; }
        public string Caption { get; set; }
        public object Value { get; set; }

        public string UIHint { get; set; }
        public bool Hide { get; set; }             
        public bool IsValid { get; set; }
        public string ErrorMessage { get; set; }
        public ViewTypes.FieldTypes ViewFieldType { get; set; }

        #region [ Constructor ]
        public FormField()
        {
            // Fields are empty by default
            Name = string.Empty;
            Caption = string.Empty;
            Value = string.Empty;
            // Fields are valid by default
            IsValid = true;
            ErrorMessage = string.Empty;
        }
        #endregion
    }

    /// <summary>
    /// List of FormField.
    /// It's just a name for better compile-type checking and semantics.
    /// </summary>
    public class FormFields : List<FormField>
    {
        /// <summary>
        /// Accesses FormField by fieldName.
        /// </summary>
        /// <param name="fieldName">field name</param>
        /// <returns>FormField instance</returns>
        public FormField this[string fieldName]
        {
            get
            {
                FormField seeked = null;
                if (this.Count > 0)
                {
                    foreach (FormField field in this)
                    {
                        if (field.Name == fieldName)
                        {
                            seeked = field;
                            break;
                        }
                    }
                }
                return seeked;
            }
        }
    }
}
