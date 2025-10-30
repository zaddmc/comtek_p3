document.addEventListener('DOMContentLoaded', () => {
  // Modal + layout elements
  const quizModal = document.getElementById('quizModal');
  const startQuizBtn = document.getElementById('startQuiz');
  const browseSuitcasesBtn = document.getElementById('browseSuitcases');
  const closeQuizBtn = document.querySelector('.close');
  const suitcasesList = document.getElementById('suitcasesList');
  const welcomeSection = document.getElementById('welcomeSection');
  const recommendationsSection = document.getElementById('recommendationsSection');
  const appShell = document.querySelector('.notuser');
  const showAllSuitcasesBtn = document.getElementById('showAllSuitcases');

  // Quiz elements
  const quizForm = document.getElementById('quizForm');
  const quizSteps = document.querySelectorAll('.quiz-step');
  const prevBtn = document.getElementById('prevBtn');
  const nextBtn = document.getElementById('nextBtn');
  const submitBtn = document.getElementById('submitQuiz');
  const stepIndicator = document.getElementById('stepIndicator');
  const selectedCategoriesInput = document.getElementById('selectedCategories');

  let currentStep = 0;
  const selectedCategoriesByStep = {}; // { stepId: [categories...] }
  let allSelectedCategories = []; // All selected categories across all steps

  // ----- Event wiring -----
  startQuizBtn?.addEventListener('click', () => {
    showModal(true);
    resetQuiz();
  });

  browseSuitcasesBtn?.addEventListener('click', () => {
    showSection('suitcases');
  });

  closeQuizBtn?.addEventListener('click', () => {
    showModal(false);
  });

  window.addEventListener('click', (evt) => {
    if (evt.target === quizModal) {
      showModal(false);
    }
  });

  showAllSuitcasesBtn?.addEventListener('click', () => {
    showSection('suitcases');
  });

  prevBtn?.addEventListener('click', () => {
    if (currentStep > 0) {
      currentStep--;
      updateStep();
    }
  });

  nextBtn?.addEventListener('click', () => {
    const stepId = quizSteps[currentStep]?.id;
    if (!stepId || !selectedCategoriesByStep[stepId]) {
      alert('Please select an answer before continuing.');
      return;
    }
    if (currentStep < quizSteps.length - 1) {
      currentStep++;
      updateStep();
    }
  });

  // Form submission - let Django handle the rendering
  quizForm?.addEventListener('submit', (evt) => {
    const stepId = quizSteps[currentStep]?.id;
    if (!stepId || !selectedCategoriesByStep[stepId]) {
      alert('Please select an answer before submitting.');
      evt.preventDefault();
      return;
    }
    
    // Update the hidden input with all selected categories
    updateSelectedCategoriesInput();
    
    // Show loading state
    if (submitBtn) {
      submitBtn.disabled = true;
      submitBtn.textContent = 'Finding your perfect match...';
    }
    
    // Form will submit normally to Django
  });

  // Option selection (event delegation)
  document.addEventListener('click', (evt) => {
    const btn = evt.target.closest('.option-btn');
    if (!btn) return;

    const questionEl = btn.closest('.quiz-step');
    if (!questionEl) return;

    // Visual selection
    questionEl.querySelectorAll('.option-btn').forEach(b => b.classList.remove('selected'));
    btn.classList.add('selected');

    // Store categories
    const stepId = questionEl.id;
    const csv = btn.dataset.categories || '';
    selectedCategoriesByStep[stepId] = csv ? csv.split(',') : [];

    updateButtonStates();
  });

  // ----- UI helpers -----
  function showModal(visible) {
    if (!quizModal || !appShell) return;
    quizModal.style.display = visible ? 'block' : 'none';
    appShell.style.display = visible ? 'none' : 'flex';
  }

  function showSection(kind) {
    // kind: 'welcome' | 'recommendations' | 'suitcases'
    if (welcomeSection) welcomeSection.style.display = kind === 'welcome' ? 'block' : 'none';
    if (recommendationsSection) recommendationsSection.style.display = kind === 'recommendations' ? 'block' : 'none';
    if (suitcasesList) suitcasesList.style.display = kind === 'suitcases' ? 'block' : 'none';
  }

  function updateStep() {
    quizSteps.forEach(step => step.classList.remove('active'));
    quizSteps[currentStep]?.classList.add('active');
    if (stepIndicator) stepIndicator.textContent = `Question ${currentStep + 1} of ${quizSteps.length}`;
    updateButtonStates();
  }

  function updateButtonStates() {
    const stepId = quizSteps[currentStep]?.id || null;
    const answered = stepId ? Boolean(selectedCategoriesByStep[stepId]) : false;

    if (prevBtn) prevBtn.disabled = currentStep === 0;

    const onLast = currentStep === quizSteps.length - 1;

    if (onLast) {
      if (nextBtn) nextBtn.style.display = 'none';
      if (submitBtn) {
        submitBtn.style.display = 'inline-block';
        submitBtn.disabled = !answered;
      }
    } else {
      if (nextBtn) {
        nextBtn.style.display = 'inline-block';
        nextBtn.disabled = !answered;
      }
      if (submitBtn) submitBtn.style.display = 'none';
    }
  }

  function updateSelectedCategoriesInput() {
    // Collect all selected categories from all steps
    allSelectedCategories = [];
    Object.values(selectedCategoriesByStep).forEach(categories => {
      if (Array.isArray(categories)) {
        allSelectedCategories.push(...categories);
      }
    });
    
    // Update the hidden input value
    if (selectedCategoriesInput) {
      selectedCategoriesInput.value = allSelectedCategories.join(',');
    }
  }

  function resetQuiz() {
    currentStep = 0;
    for (const key in selectedCategoriesByStep) delete selectedCategoriesByStep[key];
    document.querySelectorAll('.option-btn').forEach(btn => btn.classList.remove('selected'));
    if (selectedCategoriesInput) selectedCategoriesInput.value = '';
    updateStep();
  }
});