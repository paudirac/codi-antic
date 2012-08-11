using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HaleyMVCLib.ViewTypes
{
    public static class ViewFactory
    {
        public static TableView GetTableView(string table_name)
        {
            TableView table_view = null;

            Func<Type, Boolean> where_table_name = delegate(Type t)
            {
                HaleyMVCLib.ViewTypes.AssociatedEntityTypeAttribute ass_entity_type_attr = null;
                try
                {
                    ass_entity_type_attr =
                            t.GetCustomAttributes(typeof(HaleyMVCLib.ViewTypes.AssociatedEntityTypeAttribute), true)[0] as
                            HaleyMVCLib.ViewTypes.AssociatedEntityTypeAttribute;
                }
                catch (Exception) { return false; }
                if (ass_entity_type_attr == null) return false;
                else return ass_entity_type_attr.ViewName.Equals(table_name);
            };
            
            System.Reflection.Assembly assembly = System.Reflection.Assembly.GetCallingAssembly();
            var types = assembly.GetTypes()
                .Where(t => typeof(HaleyMVCLib.ViewTypes.TableView).IsAssignableFrom(t))
                .Where(where_table_name);
            if (types.Count<Type>() > 0)
            {
                var type = types.Where(where_table_name).First<Type>();
                table_view = Activator.CreateInstance(type) as TableView;
            }

            return table_view;
        }

        public static EntityFormView GetFormView(string entity_view_name, Key key)
        {
            EntityFormView form_view = null;

            Func<Type, Boolean> where_table_name = delegate(Type t)
            {
                HaleyMVCLib.ViewTypes.AssociatedEntityTypeAttribute ass_entity_type_attr = null;
                try
                {
                    ass_entity_type_attr =
                        t.GetCustomAttributes(typeof(HaleyMVCLib.ViewTypes.AssociatedEntityTypeAttribute), true)[0] as
                        HaleyMVCLib.ViewTypes.AssociatedEntityTypeAttribute;
                }
                catch (Exception) { return false; }
                if (ass_entity_type_attr == null) return false;
                else return ass_entity_type_attr.ViewName.Equals(entity_view_name);
            };
            
            System.Reflection.Assembly assembly = System.Reflection.Assembly.GetCallingAssembly();
            var types = assembly.GetTypes()
                .Where(where_table_name)
                .Where(t => typeof(HaleyMVCLib.ViewTypes.EntityFormView).IsAssignableFrom(t));
            if (types.Count<Type>() > 0)
            {
                var type = types.First<Type>();
                form_view = Activator.CreateInstance(type, new object[] { key })
                    as HaleyMVCLib.ViewTypes.EntityFormView;
            }

            return form_view;
        }

    }
}
