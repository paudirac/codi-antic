using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using HaleyMVCLib.ViewTypes.ViewAttributes;

namespace HaleyMVCLib.ViewTypes
{
    public class TableView
    {
        //public bool Filter(System.Reflection.MemberInfo mi, object type)
        //{
        //    //return mi.GetType().BaseType.Equals(type.GetType());
        //    return ((System.Type)(mi)).BaseType.Equals(type);
        //}

        Columns _columns = null;
        public Columns Columns
        {
            get
            {
                if (_columns == null)
                {
                    Type currentType = this.GetType();

                    #region [ Extract Column infromation ]

                    //Func<System.Reflection.MemberInfo, object, Boolean> search_criteria =
                    //    (mi, type) => mi.GetType() == type.GetType();
                    Func<System.Reflection.MemberInfo, object, Boolean> search_criteria =
                        (mi, type) => ((System.Type)(mi)).BaseType.Equals(type);

                    System.Reflection.MemberInfo[] member_info = currentType.FindMembers(
                        System.Reflection.MemberTypes.NestedType,
                        System.Reflection.BindingFlags.NonPublic,
                        //new System.Reflection.MemberFilter(search_criteria),
                        new System.Reflection.MemberFilter(search_criteria),
                        typeof(ViewFieldsBase));                        

                    if (member_info.Length > 0)
                    {                        
                        //Type embeded_type = Type.GetType(
                        //    String.Format("{0}+{1}", member_info[0].DeclaringType.FullName, member_info[0].Name),
                        //    true,
                        //    true);
                        Type embeded_type = currentType.Assembly.GetType(
                            String.Format("{0}+{1}", member_info[0].DeclaringType.FullName, member_info[0].Name));

                        
                        System.Reflection.FieldInfo[] view_fields_info = embeded_type.GetFields();
                        if (view_fields_info != null)
                        {
                            _columns = new Columns();
                            foreach (System.Reflection.FieldInfo view_field_info in view_fields_info)
                            {
                                string uihint = null;                                
                                try
                                {
                                    UIHintAttribute uih_attr = view_field_info.GetCustomAttributes(typeof(UIHintAttribute), false)[0]
                                        as UIHintAttribute;
                                    uihint = uih_attr.RenderTo;
                                }
                                catch { }

                                bool hide = false;
                                try
                                {
                                    HideInViewAttribute hiv_attr = view_field_info.GetCustomAttributes(typeof(HideInViewAttribute), false)[0]
                                        as HideInViewAttribute;
                                    hide = true;
                                }
                                catch { }

                                string caption = null;
                                try
                                {
                                    CaptionAttribute cap_attr = view_field_info.GetCustomAttributes(typeof(CaptionAttribute), false)[0]
                                        as CaptionAttribute;
                                    caption = cap_attr.Caption;
                                }
                                catch { }

                                _columns.Add(new ColumnInfo(FieldTypes.ViewField)
                                {
                                    Name = view_field_info.Name,
                                    Caption = caption == null? view_field_info.Name : caption,
                                    UIHint = uihint,
                                    Hide = hide
                                });
                            }
                        }
                    }

                    #endregion

                    #region [ Extract LookUpField information ]

                    //Func<System.Reflection.MemberInfo, object, Boolean> search_lookup =
                    //    (mi, type) => mi.GetType().Equals(type.GetType());
                    System.Reflection.MemberInfo[] lookup_member_info = currentType.FindMembers(
                        System.Reflection.MemberTypes.NestedType,
                        System.Reflection.BindingFlags.NonPublic,
                        new System.Reflection.MemberFilter(search_criteria),
                        typeof(RelatedFieldsBase));

                    if (lookup_member_info.Length > 0)
                    {
                        Type lookup_embedded_type = currentType.Assembly.GetType(
                            String.Format(
                                "{0}+{1}", 
                                lookup_member_info[0].DeclaringType.FullName,
                                lookup_member_info[0].Name
                            )
                        );

                        System.Reflection.FieldInfo[] lookup_fields_info = lookup_embedded_type.GetFields();
                        if (lookup_fields_info != null)
                        {
                            foreach (System.Reflection.FieldInfo lookup_field_info in lookup_fields_info)
                            {
                                _columns.Add(new ColumnInfo(FieldTypes.RelatedField)
                                {
                                    Name = lookup_field_info.Name,
                                    Caption = lookup_field_info.Name,
                                    UIHint = null,
                                    Hide = false
                                });
                            }
                        }
                    }

                    #endregion

                    if (_columns == null)
                    {
                        TableViewFieldsAttribute fields_attr =
                            this.GetType().GetCustomAttributes(typeof(TableViewFieldsAttribute), true)[0]
                            as TableViewFieldsAttribute;
                        _columns = new Columns();
                        foreach (string field_name in fields_attr.Fields)
                        {
                            _columns.Add(new ColumnInfo(FieldTypes.ViewField) { Name = field_name, Caption = field_name });
                        }

                    }
                    if (_columns == null) _columns = new Columns();
                }
                return _columns;
            }
        }

        public FlatRows FlatData
        {
            get
            {
                return GetQueryData();
            }
        }

        protected virtual FlatRows GetQueryData()
        {
            Type currentType = this.GetType();
            AssociatedDatabaseTypeAttribute db_type_attr = currentType.GetCustomAttributes(
                typeof(AssociatedDatabaseTypeAttribute), true)[0]
                as AssociatedDatabaseTypeAttribute;

            AssociatedEntityTypeAttribute ent_type_attr = currentType
                .GetCustomAttributes(typeof(AssociatedEntityTypeAttribute), true)[0]
                as AssociatedEntityTypeAttribute;

            FlatRows flat_rows = null;
            using (var db = Activator.CreateInstance(db_type_attr.DatabaseType) as IDisposable)
            {
                System.Reflection.PropertyInfo prop =
                    db.GetType().GetProperty(ent_type_attr.EntityType.Name);

                System.Reflection.MethodInfo method =
                    currentType.GetMethods(
                        System.Reflection.BindingFlags.NonPublic |
                        System.Reflection.BindingFlags.Instance)
                    .Where(mi => mi.Name == "FlattenData" && mi.IsGenericMethod)
                    .First<System.Reflection.MethodInfo>();

                //string eSQL = String.Format(
                //    "SELECT VALUE entity FROM {0}.{1} AS entity",
                //    db_type_attr.DatabaseType.Name,
                //    ent_type_attr.EntityType.Name
                //);
                //var entities = ((System.Data.Objects.ObjectContext)db).CreateQuery<

                flat_rows = (FlatRows)method.MakeGenericMethod(ent_type_attr.EntityType)
                    .Invoke(this, new object[] { prop.GetValue(db, null) });




                //flat_rows = (FlatRows)method.MakeGenericMethod(ent_type_attr.EntityType)
                //    .Invoke(this, new object[] { 
                //        prop.GetValue(db, null), 
                //        this.Columns.Where(ci => ci.FieldType == FieldTypes.ViewField)
                //    });
            }
            return flat_rows;
        }

        protected FlatRows FlattenData<T>(IQueryable<T> entity_list)//, IEnumerable<ColumnInfo> columns_to_fill)
        {
            FlatRows flattened_data = new FlatRows();
            foreach (T entity in entity_list)
            {
                #region [ Extract Key Name ]
                string key_name = "ID";
                foreach (System.Reflection.PropertyInfo prop in
                    entity.GetType().GetProperties())
                {
                    object[] attrs = prop.GetCustomAttributes(false);
                    if (attrs != null)
                    {
                        foreach (Attribute at in attrs)
                        {
                            if (at is System.Data.Objects.DataClasses.EdmScalarPropertyAttribute)
                            {
                                if (((System.Data.Objects.DataClasses.EdmScalarPropertyAttribute)at).EntityKeyProperty)
                                    key_name = prop.Name;
                            }
                        }
                    }
                    if (key_name != "ID") break;
                }
                #endregion
                Key key = new Key()
                {
                    Name = key_name,
                    Value = entity.GetType().GetProperty(key_name).GetValue(entity, null)
                };
                
                // vell
                //FlatRow row = new FlatRow(key);
                //foreach (ColumnInfo col_info in columns_to_fill)
                //    //this.Columns.Where(ci => ci.FieldType == FieldTypes.ViewField))
                //{
                //    string column = col_info.Name;
                //    object value = entity.GetType().GetProperty(column).GetValue(entity, null);
                //    row.Add(column, value != null ? value : string.Empty);
                //}
                //flattened_data.Add(row);
                
                // TODO: NOTE: amb un groupby? en FieldTypes.ViewField i FieldTypes.RelatedField

                string[] column_names =
                    this.Columns.Where<ColumnInfo>(ci => ci.FieldType == FieldTypes.ViewField)
                    .Select<ColumnInfo, string>(ci => ci.Name).ToArray<string>();
                FlatRow row = new FlatRow(key);
                foreach (string column_name in column_names)
                {
                    object value = entity.GetType().GetProperty(column_name).GetValue(entity, null);
                    row.Add(column_name, value != null ? value : string.Empty);
                }

                // TODO: loop sobre columnes, no sobre entities
                // Vigilar perquè el loop es fa per a cada entity, no és eficient, perquè
                // depend de la mida de la taula. S'ha d'iterar d'una altra forma.
                // Per columnes i entities, més que per entities i columnes. NO? Sí.
                // Probablement hi hauria d'haver una classe que s'encarregués d'això
                // o un mètode, però aquesta TableView ja té massa feina, potser.
                // TODO: canviar-ho de dalt a baix. El Load executa una query a DB cada cop
                // que es crida. S'hauria de fer un Include abans, sempre que els ViewFiels
                // fossin visibles.
                try
                {
                    System.Reflection.PropertyInfo[] relations =
                        entity.GetType().GetProperties()
                        .Where(p => p.PropertyType.BaseType.Equals(typeof(System.Data.Objects.DataClasses.EntityReference)))
                        .ToArray<System.Reflection.PropertyInfo>();

                    foreach (System.Reflection.PropertyInfo reference_prop_info in relations)
                    {
                        object reference_object = reference_prop_info.GetValue(entity, null);                        

                        int first_position = reference_prop_info.Name.IndexOf("Reference");
                        System.Reflection.PropertyInfo reference_entity_prop_info =
                            entity.GetType().GetProperty(reference_prop_info.Name.Remove(first_position));
                        object reference_entity = reference_entity_prop_info.GetValue(entity, null);

                        #region [ Per si hi ha Lazy-Load ]
                        if (reference_entity == null) /* LAZY LOAD */
                        {
                            System.Reflection.MethodInfo load_method =
                                reference_object.GetType().GetMethods()
                                .Where(mi => mi.Name.Equals("Load") && mi.GetParameters().Length == 0)
                                .First<System.Reflection.MethodInfo>();
                            load_method.Invoke(reference_object, null);
                            reference_entity = reference_entity_prop_info.GetValue(entity, null);
                        }
                        #endregion

                        string[] related_columns_names =
                            this.Columns.Where<ColumnInfo>(ci => ci.FieldType == FieldTypes.RelatedField)
                            .Select<ColumnInfo, string>(ci => ci.Name).ToArray<string>();
                        foreach (string column_name in related_columns_names)
                        {
                            object value = reference_entity.GetType().GetProperty(column_name)
                                .GetValue(reference_entity, null);
                            row.Add(column_name, value != null ? value : string.Empty);
                        }
                    }
                    flattened_data.Add(row);
                }
                catch (Exception e) { }                          
            }
            return flattened_data;
        }
    }
}
