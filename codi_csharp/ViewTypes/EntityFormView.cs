using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using HaleyMVCLib.ViewTypes.ViewAttributes;

namespace HaleyMVCLib.ViewTypes
{
    /// <summary>
    /// Proxies an Entity Framework Entity (or Entities) for the generation of form views.
    /// Provides a Validation interface for that Entity (or Entities). TODO: currently only one Entity.
    /// 
    /// The EntityFormView derived classes explore its nested classes for the generation of the fields of the
    /// entity they proxy. Specifically, the entity fields are extracted from an inner class wich is a subclass
    /// of the ViewFieldsBase class. Other fields (como los campos consulta) are extracted from another
    /// inner subclass of the type RelatedFieldsBase. TODO: currently only the "meta"-information (such as Caption).
    /// </summary>
    public class EntityFormView
    {
        protected Key Key = null;

        private FormFields _fields = null;
        public FormFields Fields
        {
            get
            {
                if (_fields == null)
                {
                    GenerateFields();
                }

                return _fields;
            }
        }

        private static System.Reflection.MethodInfo GetMethod(
            System.Linq.Expressions.Expression<Action<object>> expression)
        {
            System.Linq.Expressions.MethodCallExpression method_call =
                (System.Linq.Expressions.MethodCallExpression)expression.Body;
            return method_call.Method;
        }

        protected virtual void FillFields()
        {
            Type currentType = this.GetType();

            AssociatedDatabaseTypeAttribute db_type_attr = currentType.GetCustomAttributes(typeof(AssociatedDatabaseTypeAttribute), true)[0]
                as AssociatedDatabaseTypeAttribute;

            AssociatedEntityTypeAttribute ent_type_attr = currentType.GetCustomAttributes(typeof(AssociatedEntityTypeAttribute), true)[0]
                as AssociatedEntityTypeAttribute;
            //using (var db = new LibreriaDeEntities.MVC_devEntities())
            using (var db = Activator.CreateInstance(db_type_attr.DatabaseType) as IDisposable)
            {
                System.Reflection.PropertyInfo prop =
                    db.GetType().GetProperty(ent_type_attr.EntityType.Name);

                System.Reflection.MethodInfo method =
                    currentType.BaseType.GetMethods(
                        System.Reflection.BindingFlags.NonPublic |
                        System.Reflection.BindingFlags.Instance)
                        .Where(mi => mi.Name == "FillFields" && mi.IsGenericMethod)
                        .First<System.Reflection.MethodInfo>();

                method.MakeGenericMethod(ent_type_attr.EntityType)
                .Invoke(this, new object[] { prop.GetValue(db, null) });
            }
        }

        public virtual void Save()
        {
            Type currentType = this.GetType();

            AssociatedDatabaseTypeAttribute db_type_attr = currentType.GetCustomAttributes(typeof(AssociatedDatabaseTypeAttribute), true)[0]
                as AssociatedDatabaseTypeAttribute;

            AssociatedEntityTypeAttribute ent_type_attr = currentType.GetCustomAttributes(typeof(AssociatedEntityTypeAttribute), true)[0]
                as AssociatedEntityTypeAttribute;
            using (var db = Activator.CreateInstance(db_type_attr.DatabaseType) as IDisposable)
            {
                System.Reflection.MethodInfo method =
                    currentType.BaseType.GetMethods(
                        System.Reflection.BindingFlags.NonPublic |
                        System.Reflection.BindingFlags.Instance)
                        .Where(mi => mi.Name == "Save" && mi.IsGenericMethod)
                        .First<System.Reflection.MethodInfo>();
                method.MakeGenericMethod(ent_type_attr.EntityType)
                    .Invoke(this, new object[] { db });
            }
        }

        public virtual void Update(EntityFormView view)
        {
            Type currentType = this.GetType();

            AssociatedDatabaseTypeAttribute db_type_attr = currentType.GetCustomAttributes(typeof(AssociatedDatabaseTypeAttribute), true)[0]
                as AssociatedDatabaseTypeAttribute;

            AssociatedEntityTypeAttribute ent_type_attr = currentType.GetCustomAttributes(typeof(AssociatedEntityTypeAttribute), true)[0]
                as AssociatedEntityTypeAttribute;
            using (var db = Activator.CreateInstance(db_type_attr.DatabaseType) as IDisposable)
            {
                System.Reflection.PropertyInfo prop =
                    db.GetType().GetProperty(ent_type_attr.EntityType.Name);
                System.Reflection.MethodInfo method =
                    currentType.BaseType.GetMethods(
                        System.Reflection.BindingFlags.NonPublic |
                        System.Reflection.BindingFlags.Instance)
                        .Where(mi => mi.Name == "Update" && mi.IsGenericMethod)
                        .First<System.Reflection.MethodInfo>();
                method.MakeGenericMethod(ent_type_attr.EntityType)
                    .Invoke(this, new object[] { 
                        db,
                        prop.GetValue(db, null),
                        view
                    });
            }
        }

        public virtual void Delete()
        {
            Type currentType = this.GetType();

            AssociatedDatabaseTypeAttribute db_type_attr = currentType.GetCustomAttributes(typeof(AssociatedDatabaseTypeAttribute), true)[0]
                as AssociatedDatabaseTypeAttribute;

            AssociatedEntityTypeAttribute ent_type_attr = currentType.GetCustomAttributes(typeof(AssociatedEntityTypeAttribute), true)[0]
                as AssociatedEntityTypeAttribute;
            using (var db = Activator.CreateInstance(db_type_attr.DatabaseType) as IDisposable)
            {
                System.Reflection.PropertyInfo prop =
                    db.GetType().GetProperty(ent_type_attr.EntityType.Name);
                System.Reflection.MethodInfo method =
                    currentType.BaseType.GetMethods(
                        System.Reflection.BindingFlags.NonPublic |
                        System.Reflection.BindingFlags.Instance)
                        .Where(mi => mi.Name == "Delete" && mi.IsGenericMethod)
                        .First<System.Reflection.MethodInfo>();
                method.MakeGenericMethod(ent_type_attr.EntityType)
                    .Invoke(this, new object[] { 
                        db,
                        prop.GetValue(db, null)
                    });
            }

        }

        protected void FillFields<T1>(IQueryable<T1> iQueryable)
        {
            IEnumerable<T1> entity_enum = GetEntity<T1>(iQueryable);
            FillView<T1>(entity_enum);
        }

        private void GenerateFields()
        {
            if (_fields == null)
            {
                Type currentType = this.GetType();
                Func<System.Reflection.MemberInfo, object, Boolean> search_criteria =
                    (mi, type) => mi.GetType() == type.GetType();
                System.Reflection.MemberInfo[] member_info = currentType.FindMembers(
                    System.Reflection.MemberTypes.NestedType,
                    System.Reflection.BindingFlags.NonPublic,
                    new System.Reflection.MemberFilter(search_criteria),
                    typeof(ViewFieldsBase));

                if (member_info.Length > 0)
                {
                    //Type embedded_type = Type.GetType(
                    //    String.Format("{0}+{1}", member_info[0].DeclaringType.FullName, member_info[0].Name));
                    Type embedded_type = currentType.Assembly.GetType(
                        String.Format("{0}+{1}", member_info[0].DeclaringType.FullName, member_info[0].Name));
                    System.Reflection.FieldInfo[] fields = embedded_type.GetFields();
                    if (fields != null)
                    {
                        _fields = new FormFields();
                        foreach (System.Reflection.FieldInfo field in fields)
                        {
                            bool hide = false;                            
                            try
                            {
                                HideInViewAttribute hiv_attr = field.GetCustomAttributes(typeof(HideInViewAttribute), false)[0]
                                    as HideInViewAttribute;
                                hide = true;
                            }
                            catch { }

                            string uihint = null;
                            try
                            {
                                UIHintAttribute uih_attr = field.GetCustomAttributes(typeof(UIHintAttribute), false)[0]
                                    as UIHintAttribute;
                                uihint = uih_attr.RenderTo;
                            }
                            catch { }

                            string caption = null;
                            try
                            {
                                CaptionAttribute cap_attr = field.GetCustomAttributes(typeof(CaptionAttribute), false)[0]
                                    as CaptionAttribute;
                                caption = cap_attr.Caption;
                            }
                            catch { }

                            _fields.Add(new FormField()
                            {
                                // UIHint
                                Caption = caption == null? field.Name : caption,
                                Name = field.Name,
                                Value = null,
                                Hide = hide,
                                UIHint = uihint
                            });
                        }
                    }
                }

                // No inner class found
                if (_fields == null)
                {
                    TableViewFieldsAttribute fields_attr =
                        this.GetType().GetCustomAttributes(typeof(TableViewFieldsAttribute), true)[0]
                        as TableViewFieldsAttribute;
                    _fields = new FormFields();
                    foreach (string field in fields_attr.Fields)
                    {
                        FormField form_field = new FormField();
                        form_field.Caption = field;
                        form_field.Name = field;
                        form_field.Value = null;

                        _fields.Add(form_field);
                    }
                }
            }
        }

        protected IEnumerable<T> GetEntity<T>(System.Linq.IQueryable<T> query)
        {
            EnsureKeyNameIsSet<T>();
            System.Reflection.PropertyInfo prop = typeof(T).GetProperty(Key.Name);

            if (Key.Value.GetType() != prop.PropertyType)
            {
                // assume Key.Value is a string coming from a WebPage                
                Key.Value = Convert.ChangeType(Key.Value, prop.PropertyType);
                //if (prop.PropertyType == typeof(Int32))
                //    Key.Value = Convert.ToInt32(Key.Value);
            }

            Func<T, Boolean> where = x => prop.GetValue(x, null).Equals(Key.Value);
            return query.Where<T>(where);
        }

        private void EnsureKeyNameIsSet<T>()
        {
            if (Key.Name == null || Key.Name.Equals(string.Empty))
            {
                #region [ Extract Key Name ]
                string key_name = "ID";
                foreach (System.Reflection.PropertyInfo p in
                    typeof(T).GetProperties())
                {
                    object[] attrs = p.GetCustomAttributes(false);
                    if (attrs != null)
                    {
                        foreach (Attribute at in attrs)
                        {
                            if (at is System.Data.Objects.DataClasses.EdmScalarPropertyAttribute)
                            {
                                if (((System.Data.Objects.DataClasses.EdmScalarPropertyAttribute)at).EntityKeyProperty)
                                    key_name = p.Name;
                            }
                        }
                    }
                    if (key_name != "ID") break;
                }
                Key.Name = key_name;
                #endregion
            }
        }

        private void FillView<T>(IEnumerable<T> entity)
        {
            //TableViewFieldsAttribute fields_attr =
            //        this.GetType().GetCustomAttributes(typeof(TableViewFieldsAttribute), true)[0]
            //        as TableViewFieldsAttribute;

            T typed_entity = entity.Cast<T>().First<T>();

            foreach (FormField field in Fields)
            {
                System.Reflection.PropertyInfo prop = typeof(T).GetProperty(field.Name);
                object value = prop.GetValue(typed_entity, null);
                field.Value = value;
            }
        }

        protected void Update<T>(System.Data.Objects.ObjectContext db, System.Linq.IQueryable<T> query, EntityFormView changes)
        {
            EnsureKeyNameIsSet<T>();
            System.Reflection.PropertyInfo key_prop = typeof(T).GetProperty(Key.Name);
            //object key_value = changes.Fields[Key.Name].Value;
            object key_value = Key.Value;
            if (key_value.GetType() != key_prop.PropertyType)
            {
                try
                {
                    key_value = Convert.ChangeType(key_value, key_prop.PropertyType);
                    //if (key_prop.PropertyType == typeof(Int32))
                    //{
                    //    key_value = Convert.ToInt32(key_value);
                    //}
                }
                catch (Exception e) { }
            }
            Func<T, Boolean> where =
                x => key_prop.GetValue(x, null).Equals(key_value);

            T db_obj = query.Where<T>(where).First<T>();
            foreach (FormField field in this.Fields)
            {
                System.Reflection.PropertyInfo prop =
                    typeof(T).GetProperty(field.Name);
                object db_value = prop.GetValue(db_obj, null);
                object new_value = changes.Fields[prop.Name].Value;

                if (field.Name.Equals(Key.Name)) continue;

                if (new_value.GetType() != prop.PropertyType)
                {
                    try
                    {
                        System.ComponentModel.TypeConverter tc =
                            System.ComponentModel.TypeDescriptor.GetConverter(prop.PropertyType);
                        if ((prop.PropertyType.Equals(typeof(Double)) || prop.PropertyType.Equals(typeof(Nullable<Double>))) 
                            && new_value.GetType().Equals(typeof(string)))
                        {
                            new_value = ((string)new_value).Replace(".", ",");
                        }
                        if (tc.CanConvertFrom(new_value.GetType()))
                        {
                            new_value = tc.ConvertFrom(new_value);
                        }
                        else throw new Exception();
                    }
                    catch (Exception)
                    {
                        throw new Exception(
                            String.Format("Invalid input. The value {0} is not allowed in the field {1}.",
                                new_value.Equals(string.Empty) ? "NULL" : new_value, field.Name));
                    }
                }
                // NOTA: si ho fem així, l'EF només genera l'SQL per a aquelles properties que han canviat (hem passat pel setter)
                // i només envia aquesta informació a la DB. Si fessim prop.SetValue(db_obj, new_value, null), sense l'*if*, llavors
                // generaria codi per a totes les properties, doncs hem passat pel setter.                
                if (db_value == null || db_value != null && !db_value.Equals(new_value))
                    prop.SetValue(db_obj, new_value, null);
            }

            db.SaveChanges();
        }

        protected void Save<T>(System.Data.Objects.ObjectContext db)
        {
            AssociatedEntityTypeAttribute entity_type_attr =
                this.GetType().GetCustomAttributes(typeof(AssociatedEntityTypeAttribute), true)[0]
                as AssociatedEntityTypeAttribute;
            T entity = (T)Activator.CreateInstance(typeof(T));
            EnsureKeyNameIsSet<T>();
            foreach (FormField field in this.Fields)
            {                
                System.Reflection.PropertyInfo prop = entity_type_attr.EntityType.GetProperty(field.Name);
                object value = field.Value;

                if (field.Name.Equals(Key.Name)) continue;

                if (value.GetType() != prop.PropertyType)
                {
                    try
                    {
                        System.ComponentModel.TypeConverter tc =
                            System.ComponentModel.TypeDescriptor.GetConverter(prop.PropertyType);
                        if (tc.CanConvertFrom(value.GetType()))
                        {
                            value = tc.ConvertFrom(value);
                        }
                        else throw new Exception();
                        //new_value = Convert.ChangeType(new_value, prop.PropertyType);
                    }
                    catch (Exception)
                    {
                        throw new Exception(
                            String.Format("Invalid input. The value {0} is not allowed in the field {1}.",
                                value.Equals(string.Empty) ? "NULL" : value, field.Name));
                    }
                }
                prop.SetValue(entity, value, null);
            }

            // Recover save method from ObjectContext and add object
            
            System.Reflection.MethodInfo addMethod = db.GetType().GetMethod(
                String.Format("AddTo{0}", entity_type_attr.EntityType.Name));
            addMethod.Invoke(db, new object[] { entity });

            try
            {
                db.SaveChanges();
            }
            catch (Exception e) { }
        }

        protected void Delete<T>(System.Data.Objects.ObjectContext db, System.Linq.IQueryable<T> query)
        {
            System.Reflection.PropertyInfo prop = typeof(T).GetProperty(Key.Name);
            object key_value = this.Fields[prop.Name].Value;
            if (Key.Value.GetType() != prop.PropertyType)
            {
                key_value = Convert.ChangeType(key_value, prop.PropertyType);
                //if (prop.PropertyType == typeof(Int32))
                //    key_value = Convert.ToInt32(key_value);
            }
            Func<T, Boolean> where =
                x => prop.GetValue(x, null).Equals(key_value);

            db.DeleteObject(query.Where<T>(where).First<T>());
            db.SaveChanges();
        }

        public void Validate()
        {
            // TODO: refactorizar todo este tinglado se repite
            // potser passar-ho a una classe que s'encarregui de fer el
            // traspàs de coses de l'objecte al form
            Type currentType = this.GetType();
                Func<System.Reflection.MemberInfo, object, Boolean> search_criteria =
                    (mi, type) => mi.GetType() == type.GetType();
                System.Reflection.MemberInfo[] member_info = currentType.FindMembers(
                    System.Reflection.MemberTypes.NestedType,
                    System.Reflection.BindingFlags.NonPublic,
                    new System.Reflection.MemberFilter(search_criteria),
                    typeof(ViewFieldsBase));

                if (member_info.Length > 0)
                {
                    Type embedded_type = currentType.Assembly.GetType(
                        String.Format("{0}+{1}", member_info[0].DeclaringType.FullName, member_info[0].Name));
                    System.Reflection.FieldInfo[] fields = embedded_type.GetFields();
                    if (fields != null)
                    {
                        foreach (System.Reflection.FieldInfo field in fields)
                        {
                            HaleyMVCLib.Validators.BaseValidatorAttribute validator = null;
                            // Find validators
                            try
                            {
                                object[] validators_attr = field.GetCustomAttributes(typeof(HaleyMVCLib.Validators.BaseValidatorAttribute), true);
                                if (validators_attr.Length > 1)
                                {
                                    validator =
                                        new HaleyMVCLib.Validators.ChainedValidatorAttribute(
                                            (HaleyMVCLib.Validators.BaseValidatorAttribute[])validators_attr);
                                }
                                else if (validators_attr.Length == 1)
                                {
                                    validator = (HaleyMVCLib.Validators.BaseValidatorAttribute)validators_attr[0];
                                }
                            }
                            catch { }

                            FormField ffield = Fields[field.Name];

                            // Validate
                            if (validator != null)
                            {
                                
                                string atempted_value = ffield.Value as string;
                                validator.ValidateField(ffield, atempted_value);
                            }
                            else
                            {
                                ffield.IsValid = true;
                                ffield.ErrorMessage = string.Empty;
                            }
                        }
                    }
                }
        }
        
        public bool IsValid
        {
            get
            {
                Validate();
                bool isValid = true;
                foreach (FormField field in Fields)
                {
                    isValid = isValid && field.IsValid;
                    if (!isValid) break;
                }
                return isValid;
            }

        }

        public EntityFormView(Key key)
        {
            Key = key;
            //if (Key == null) GenerateFields();
            if (Key != null &&
                Key.Value != null &&
                !Key.Value.Equals(string.Empty))
            {
                FillFields();
            }
        }
    }
}
