using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Supervision.Clases;

namespace Supervision
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }
        private float Distancia = 1000; //variable para controlar si el dispositivo se está moviendo
        private void Form1_Load(object sender, EventArgs e)
        {   //Crea la vista de la lista y las columnas con sus titulos

            listView1.View = View.Details;
            listView1.Columns.Add("Desplazamientos:", 100, HorizontalAlignment.Left);
            listView1.Columns.Add("Usuario", 100, HorizontalAlignment.Left);
            listView1.Columns.Add("Fecha", 150, HorizontalAlignment.Left);
            listView1.Columns.Add("Estancia", 100, HorizontalAlignment.Left);
            listView1.Columns.Add("Movimiento", 100, HorizontalAlignment.Left);
            listView1.Columns.Add("Distancia", 100, HorizontalAlignment.Left);
            Init_timer1(); //inicia el timer
        }
        private void Init_timer1()
        {
            timer1.Interval = 60000; //Intervalo de 1 minuto = 60000 milisegundos
            timer1.Tick += new EventHandler(Timer1_Tick);
            timer1.Enabled = true;
            button1.Click += new EventHandler(button1_Click);
        }
        private void AvisarLlegada(String llego, String usuario)
        {            
            String llegada = llego;
            if (llegada == "parar")
            {
                label1.Text = "El usuario: " + usuario + " ha llegado a destino. ";
                MessageBox.Show("El usuario: " + usuario + "ha llegado a destino.", "Compruebe si la estancia es correcta.", MessageBoxButtons.OKCancel);
            }
        }
        private void AvisarProblema(float distancia, String llego, String usuario)
        {
            
            if ((Distancia == distancia) && (llego!="parar")) // El usuario se ha detenido y no ha llegado al final
            {
                label1.Text = "El usuario tiene algún problema, no ha parado y la distancia recorrida es la misma.";
                MessageBox.Show("¡ATENCION! PROBLEMAS!","Revise desplazamiento usuario:", MessageBoxButtons.OKCancel);
            }
            else
            {
                Distancia = distancia;
            }
        }
        private async void Timer1_Tick(object sender, EventArgs e) //recoje los datos cada tick del reloj
        {
            try
            {
                Connection cliente = new Connection();
                var datos = await cliente.GetData();
                if (datos != null)
                {
                    ListViewItem item = new ListViewItem(); //Muestra los datos recibidos en un lista
                    foreach (Datos i in datos) 
                    {
                        item.SubItems.Add(i.Usuario);
                        item.SubItems.Add(i.Fecha.ToString());
                        item.SubItems.Add(i.Estancia.ToString());
                        item.SubItems.Add(i.Movimiento.ToString());
                        item.SubItems.Add(i.Distancia.ToString());
                        AvisarLlegada(i.Movimiento.ToString(), i.Usuario); // Avisa si el usuario ha llegado a destino                                 
                        AvisarProblema(i.Distancia,i.Movimiento.ToString(),i.Usuario); // Avisa si hay algún problema
                    }                    
                    listView1.Items.Add(item);
                }
                else
                {
                    label1.Text = "No hay datos de usuarios en las instalaciones. ";
                }
            }
            catch (Exception e1)
            {
                Console.WriteLine("Error al crear la conexión y recibir datos : " + e1.ToString());
            }
            
        }
        
        private void button1_Click(object sender, EventArgs e) //Para y reanuda la recepcion de datos
        {
            if (button1.Text == "Parar")
            {
                timer1.Enabled = true;
                button1.Text = "Reanudar";
            }
            else
            {
                timer1.Enabled = false;
                button1.Text = "Parar";
            }
                    
        }

       
    }
}
