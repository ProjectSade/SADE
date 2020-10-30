using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Essentials;
using Xamarin.Forms;
using Xamarin.Forms.PlatformConfiguration.AndroidSpecific.AppCompat;
using Interfaz.Clases;


namespace Interfaz
{
    public partial class MainPage : ContentPage
    {
        private bool seguirTimer = true; // para iniciar el timer 
        private bool seguirTimer2 = true; //para iniciar el timer 2
        public MainPage()
        {
            InitializeComponent();
        }
        
        private async void ObtenerDatos() //metodo que se invoca para leer datos si el usuario ha llegado, deja de recibir datos y para el timer
        {
            Connection consulta = new Connection();
            try
            {
                 Datos datos = await consulta.GetDatos();
                 if (datos.Movimiento == 0) //si el usuario ha llegado para la lectura e inicia el segundo timer
                    {
                        seguirTimer = false; //detiene el timer 1
                        seguirTimer2 = true; //reinicia segundo timer por si hay un segundo itinerario
                        await Navigation.PushAsync(new Final()); //muestra la pagina final
                        Device.StartTimer(TimeSpan.FromSeconds(35), () => //espera 2 minutos y vuelve a inicio
                        {
                        if (seguirTimer2 == false)
                            {
                            return false;
                            }
                            VolverMainPage();
                            return true;
                        });
                }
                

            }
            catch (Exception e1)
            {
                Console.WriteLine("Excepción en la consulta: " + e1.Message);
            }
        }
        private async void VolverMainPage() //funcion que desapila 2 paginas de la pila para volver a inicio
        {
            await Navigation.PopAsync();
            await Navigation.PopAsync();
            seguirTimer2 = false; //para el segundo timer
            seguirTimer = true; //reinicia el primer timer para sucesivos itinerarios
        }

        async void btnSala1Clicked(object sender, EventArgs e)
        {
            await Navigation.PushAsync(new Progreso());
            object objIns = new { pet = "inserta sala", user = "prototipo", sala = "1" };
            Connection inserta = new Connection();
            try { 
            bool exito = await inserta.SetSala(objIns); //inserta el usuario y la sala
                if (exito)
                {
                    Device.StartTimer(TimeSpan.FromSeconds(20), () =>  
                    {
                        if (seguirTimer == false)
                        {                           
                            return false;
                        }
                        ObtenerDatos();
                        return true;
                    });
                   
                }

            }
            catch (Exception e2)
            {
                Console.WriteLine("Excepción en la inserción: " + e2.Message);
            }
            
            
           
        }
        async void btnSalidaClicked(object sender, EventArgs e)
        {
            await Navigation.PushAsync(new Progreso());
            object objIns = new { pet = "inserta sala", user = "prototipo", sala = "0" };
            Connection inserta = new Connection();
            try
            {
                bool exito = await inserta.SetSala(objIns); //inserta el usuario y la sala
                if (exito)
                {
                    Device.StartTimer(TimeSpan.FromSeconds(20), () =>
                    {
                        if (seguirTimer == false)
                        {
                            return false;
                        }
                        ObtenerDatos();
                        return true;
                    });
                   
                }

            }
            catch (Exception e2)
            {
                Console.WriteLine("Excepción en la inserción: " + e2.Message);
            }        
            

        }

    }
}

//await Navigation.PopAsync();
//await Navigation.PopAsync();


